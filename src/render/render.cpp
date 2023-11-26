#include "render.hpp"

using namespace std;

namespace tc {

// public:

Render::Render(int p_X_size, int p_Y_size) : X_size(p_X_size), Y_size(p_Y_size) {
    clear_buffers();
}

void Render::render(mesh m) {
    clear_buffers();
    execute_vertex_shader(&m, vert_shaders::VERT_camera);
    rasterize(&m);
    execute_fragment_and_post_shaders(frag_shaders::FRAG_shaded, post_shaders::POST_vignette);
    draw_fbuf();
}

void Render::set_debug_info(std::string debug_info) {
    debug_buf.clear(X_size, Y_size, ' ');

    int x = 0, y = 0;
    for (int i = 0; i < debug_info.length(); ++i) {
        if (debug_info[i] == '\n') {
            ++y;
            x = 0;
        } else {
            debug_buf.buf[x][y] = debug_info[i];
            ++x;
        }
        if (x > X_size || y > Y_size) break;
    }
}

void Render::set_params(int p_X_size, int p_Y_size, float p_global_time, float p_time_of_day, glm::mat4 p_V, glm::mat4 p_VP, block_type::Block_Type p_active_block_type) {
    X_size = p_X_size;
    Y_size = p_Y_size;
    global_time = p_global_time;
    time_of_day = p_time_of_day;
    time_of_day_update();
    VP = p_VP;
    V = p_V;
    active_block_type = p_active_block_type;
}

void Render::get_params(int *n_tris_ptr, int *n_active_tris_ptr) {
    *n_tris_ptr = n_tris;
    *n_active_tris_ptr = n_active_tris;
}

// private:

void Render::time_of_day_update() {
    const float two_pi = 6.283185307f;

    sun_direction = glm::vec3 {
        -sin(time_of_day * two_pi),
        cos(time_of_day * two_pi),
        0.0f
    };
    sky_brightness = glm::clamp(-cos(time_of_day * two_pi) * 1.3f, 0.0f, 1.0f) * 0.9f + 0.1f;
}

void Render::clear_buffers() {
    fbuf.clear(X_size, Y_size, U.sky_color * sky_brightness);
    frag_buf.clear(X_size, Y_size, list<fragment> {});
    hud_buf.clear(X_size, Y_size, " ");
    // NOT clearing debug_buf, already set by set_debug_info()
}

void Render::execute_vertex_shader(mesh *m, void (*vert_shader)(vertex*, glm::mat4, glm::mat4, float)) {
    n_tris = m->tri_list.size(); // for debug info

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < m->tri_list.size(); ++i) {
        tri &triangle = m->tri_list[i];

        for (vertex &v : triangle.vertices) {
            // Programmable Shader
            vert_shader(&v, V, VP, global_time);

            /* Depth Division
             * pre-divides w too so that we can simply multiply in perspective
             * correction (for performance; following OpenGL spec) */
            v.pos = glm::vec4(v.pos.xyz(), 1.0f) / v.pos.w;
        }

        /* View Clipping and Backface Culling
         * If the triangle doesn't touch NDC space (enough)
         * or is facing away from the camera,
         * it is (usually) marked for death. */
        triangle.view_normal = triangle.calc_normal();

        bool backfacing = glm::sign(triangle.view_normal.z) >= 0;

        if (!draw_util::is_tri_in_NDC(triangle) ||
            backfacing && !U.bad_normals && !block_type::block_transparent[triangle.block_ptr->type]) {

            triangle.marked_for_death = true;

        } else {
            /* backfacing normal correction */
            if (backfacing) {
                triangle.world_normal *= -1.0f;
            }

            // screen transform
            for (vertex &v : triangle.vertices) {
                v.screenpos = v.pos * 0.5f + 0.5f;
                v.screenpos.x *= X_size;
                v.screenpos.y *= Y_size;
            }
        }
    }

    /* View Clipping and Backface Culling
     * build a new mesh without marked-for-death triangles
     * (faster than erasing individually) */
    mesh tmp;
    for (int i = 0; i < m->tri_list.size(); ++i) {
        if (!m->tri_list[i].marked_for_death) {
            tmp.tri_list.push_back(m->tri_list[i]);
        }
    }
    *m = tmp;

    n_active_tris = m->tri_list.size(); // for debug info
}

void Render::rasterize(mesh *m) {
    // std::sort(m->tri_list.begin(), m->tri_list.end(), [](tri a, tri b) {
    //     float depth_a = (a.vertices[0].pos.z + a.vertices[1].pos.z + a.vertices[2].pos.z) / 3.0f;
    //     float depth_b = (b.vertices[0].pos.z + b.vertices[1].pos.z + b.vertices[2].pos.z) / 3.0f;
    //     return depth_a > depth_b;
    // });

    #pragma omp parallel for schedule(static)
    for (tri &triangle : m->tri_list) {

        // find bounding box
        int min_x = max(min(min(triangle.vertices[0].screenpos.x,
                                triangle.vertices[1].screenpos.x),
                            triangle.vertices[2].screenpos.x),
                        0.0f);
        int max_x = min(max(max(triangle.vertices[0].screenpos.x,
                                triangle.vertices[1].screenpos.x),
                            triangle.vertices[2].screenpos.x),
                        static_cast<float>(X_size));
        int min_y = max(min(min(triangle.vertices[0].screenpos.y,
                                triangle.vertices[1].screenpos.y),
                            triangle.vertices[2].screenpos.y),
                        0.0f);
        int max_y = min(max(max(triangle.vertices[0].screenpos.y,
                                triangle.vertices[1].screenpos.y),
                            triangle.vertices[2].screenpos.y),
                        static_cast<float>(Y_size));

        // integer coordinates
        glm::ivec2 p0 = triangle.vertices[0].screenpos;
        glm::ivec2 p1 = triangle.vertices[1].screenpos;
        glm::ivec2 p2 = triangle.vertices[2].screenpos;

        /* pre-calculate area for barycentric coordinates
         * reference: https://ceng2.ktu.edu.tr/~cakir/files/grafikler/Texture_Mapping.pdf */
        float area = draw_util::cc_signed_area(p0, p1, p2);

        // iterate through pixels
        for (int x = min_x; x < max_x; ++x) {
            for (int y = min_y; y < max_y; ++y) {
                glm::ivec2 p {x, y};

                /* calculate barycentric coordinates
                 * reference: https://ceng2.ktu.edu.tr/~cakir/files/grafikler/Texture_Mapping.pdf */
                float u = draw_util::cc_signed_area(p, p1, p2) / area;
                float v = draw_util::cc_signed_area(p, p2, p0) / area;
                /* This method doesn't work well for some reason:
                 *  float w = 1.0f - u - v;
                 * Therefore we calculate it with the standard approach: */
                float w = draw_util::cc_signed_area(p, p0, p1) / area;

                /* perspective-corrected barycentric coordinates
                 * reference: https://stackoverflow.com/questions/24441631/how-exactly-does-opengl-do-perspectively-correct-linear-interpolation */
                float b0 = u * triangle.vertices[0].pos.w;
                float b1 = v * triangle.vertices[1].pos.w;
                float b2 = w * triangle.vertices[2].pos.w;
                float inv_b_sum = 1.0f / (b0 + b1 + b2);
                b0 *= inv_b_sum;
                b1 *= inv_b_sum;
                b2 *= inv_b_sum;

                /* checking if point is inside triangle */
                if (b0 >= 0 && b1 >= 0 && b2 >= 0) {
                    // interpolate depth
                    float z = b0 * triangle.vertices[0].pos.z
                            + b1 * triangle.vertices[1].pos.z
                            + b2 * triangle.vertices[2].pos.z;

                    // interpolate alpha
                    const Texture_Set *tex_set = (triangle.block_ptr->type < 0 ||
                                                  triangle.block_ptr->type >= std::extent<decltype(block_type::block_texture)>::value) ?
                                                  &block_type::block_texture[0] :
                                                  &block_type::block_texture[triangle.block_ptr->type];
                    float a = U.disable_textures ?
                              1.0f :
                              tex_set->sample(b0 * triangle.vertices[0].tex_coord
                                            + b1 * triangle.vertices[1].tex_coord
                                            + b2 * triangle.vertices[2].tex_coord,
                                              triangle.block_side_index).a;

                    // create fragment if depth test passes
                    #pragma omp critical
                    {
                        auto ins_point = frag_buf.buf[x][y].begin();
                        auto i = frag_buf.buf[x][y].begin();
                        while (i != frag_buf.buf[x][y].end()) {
                            if (z < (*i).depth) {
                                ins_point = i;
                                break;
                            }
                            else if (i == std::prev(frag_buf.buf[x][y].end())) {
                                ins_point = frag_buf.buf[x][y].end();
                                break;
                            }
                            ++i;
                        }
                        i = frag_buf.buf[x][y].begin();
                        bool occluded = false;
                        while (i != ins_point) {
                            if ((*i).opacity == 1.0f) {
                                occluded = true;
                                break;
                            }
                            ++i;
                        }
                        if (!occluded) {
                            auto end = frag_buf.buf[x][y].emplace(ins_point, &triangle, b0, b1, b2, z, a);
                            if (a == 1.0f) {
                                frag_buf.buf[x][y].erase(ins_point, frag_buf.buf[x][y].end());
                            }
                        }
                    }
                }
            }
        }
    }
}

void Render::execute_fragment_and_post_shaders(glm::vec3 (*frag_shader)(fragment, glm::vec3, float, float),
                                               glm::vec3 (*post_shader)(const buffer<glm::vec3>*, glm::ivec2, glm::ivec2, float)) {
    #pragma omp parallel for schedule(static) collapse(2)
    for (int x = 0; x < X_size; ++x) {
        for (int y = 0; y < Y_size; ++y) {
            // Programmable Fragment Shader
            for (auto i = frag_buf.buf[x][y].rbegin(); i != frag_buf.buf[x][y].rend(); ++i) {
                fbuf.buf[x][y] = glm::mix(fbuf.buf[x][y], frag_shader((*i), sun_direction, sky_brightness, global_time), (*i).opacity);
            }

            // Programmable Post Processing Shader
            fbuf.buf[x][y] = post_shader(&fbuf, {x, y}, {X_size, Y_size}, global_time);

            // Construct HUD
            // crosshair
            if (x == X_size / 2 && y == Y_size / 2) {
                hud_buf.buf[x][y] = string{}
                                    .append(draw_util::auto_color_string(draw_util::FG, glm::vec3(1.0f)))
                                    .append("\x1b[1m+");
            }
            // debug info
            if (U.debug_info) {
                if (debug_buf.buf[x][y] != ' ') {
                    hud_buf.buf[x][y] = string{}
                                        .append(draw_util::auto_color_string(draw_util::FG, glm::vec3(1.0f)))
                                        .append(string {debug_buf.buf[x][y]});
                }
            }
        }
    }

    // Construct HUD
    // block selector
    for (int i = 1; i < std::extent<decltype(block_type::block_color)>::value && Y_size-2 - i >= 0; ++i) {
        std::string font_style = string {"\x1b[1"}
                                 .append((i == active_block_type) ? ";7m" : "m")
                                 .append(draw_util::auto_color_string(draw_util::FG,  glm::vec3(1.0f)));
        std::string middle_part = string{}
                                  .append(U.color_mode == "ASCII" ?
                                      string {block_type::block_initial[i]} :
                                      draw_util::auto_color_string(draw_util::BG,  block_type::block_color[i]).append(" ")
                                  )
                                  .append(draw_util::ansi_clear_string());
        hud_buf.buf[0][Y_size-2 - i] = string{}
                                       .append(font_style)
                                       .append(to_string(i));
        hud_buf.buf[1][Y_size-2 - i] = string{}
                                       .append(font_style)
                                       .append("[");
        hud_buf.buf[2][Y_size-2 - i] = middle_part;
        hud_buf.buf[3][Y_size-2 - i] = middle_part;
        hud_buf.buf[4][Y_size-2 - i] = string{}
                                       .append(font_style)
                                       .append("]");
    }
}

void Render::draw_fbuf() {
    string printbuf = "";

    for (size_t y = 0; y < Y_size; y++) {
        if(y) printbuf.append("\n");
        for (size_t x = 0; x < X_size; x++) {
            if (U.color_mode == "ASCII") {
                if (hud_buf.buf[x][y] == " ") printbuf.append(draw_util::ascii_bw_color_string(draw_util::BG, fbuf.buf[x][y]));
                else printbuf.append(hud_buf.buf[x][y]);
            } else {
                printbuf.append(draw_util::auto_color_string(draw_util::BG, fbuf.buf[x][y]));
                printbuf.append(hud_buf.buf[x][y]);
            }
            printbuf.append(draw_util::ansi_clear_string());
        }
    }

    printf("%s", printbuf.c_str());
    fflush(stdout);
}

} /* end of namespace tc */
