#include "../glm.hpp"

#include "render.hpp"
#include "buffer.hpp"
#include "draw_util.hpp"
#include "mesh.hpp"
#include "../shaders/vert_shaders.hpp"
#include "../shaders/frag_shaders.hpp"

#include <string>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <array>
#include <optional>

using namespace std;

namespace tc {

Render::Render(int p_X_size, int p_Y_size) : X_size(p_X_size), Y_size(p_Y_size) {
    clear_buffers();
}

void Render::render(mesh m) {
    clear_buffers();

    // m = mesh {};
    // m.tri_list.emplace_back(vertex(-0.5, 0.5, 0.0), vertex(0.5, 0.5, 0.0), vertex(0.0, -0.5, 0.0));
    // m.tri_list[0].vertices[0].color = glm::vec3(1.0f, 0.0f, 0.0f);
    // m.tri_list[0].vertices[1].color = glm::vec3(0.0f, 1.0f, 0.0f);
    // m.tri_list[0].vertices[2].color = glm::vec3(0.0f, 0.0f, 1.0f);

    execute_vertex_shader(&m, vert_shaders::VERT_camera);
    rasterize(&m);
    execute_fragment_shader(frag_shaders::FRAG_fun);

    draw_fbuf();
}

void Render::set_params(int p_X_size, int p_Y_size, float p_global_time, glm::mat4 p_VP) {
    X_size = p_X_size;
    Y_size = p_Y_size;
    global_time = p_global_time;
    VP = p_VP;
}

void Render::get_params(int *n_tris_ptr, int *n_active_tris_ptr) {
    *n_tris_ptr = n_tris;
    *n_active_tris_ptr = n_active_tris;
}

void Render::clear_buffers() {
    fbuf.clear(X_size, Y_size, glm::vec3(0.0f));
    zbuf.clear(X_size, Y_size, 1.0f);
    frag_buf.clear(X_size, Y_size, optional<fragment>{});
}

void Render::execute_vertex_shader(mesh *m, void (*vert_shader)(vertex*, glm::mat4, float)) {
    n_tris = m->tri_list.size(); // for debug info

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < m->tri_list.size(); ++i) {
        tri &triangle = m->tri_list[i];

        for (vertex &v : triangle.vertices) {
            // Programmable Shader
            vert_shader(&v, VP, global_time);
        }

        for (vertex &v : triangle.vertices) {
            /* Depth Division
             * pre-divides w too so that we can simply multiply in perspective
             * correction (for performance; following OpenGL spec) */
            v.pos = glm::vec4(v.pos.xyz(), 1.0f) / v.pos.w;
        }

        /* View Clipping
         * If every vertex is outside of NDC space, the triangle
         * is marked for death. */
        if (glm::any(glm::greaterThan(glm::abs(triangle.vertices[0].pos.xyz()), glm::vec3(1.0f))) &&
            glm::any(glm::greaterThan(glm::abs(triangle.vertices[1].pos.xyz()), glm::vec3(1.0f))) &&
            glm::any(glm::greaterThan(glm::abs(triangle.vertices[2].pos.xyz()), glm::vec3(1.0f)))) {

            triangle.marked_for_death = true;

        } else {
            // screen transform
            for (vertex &v : triangle.vertices) {
                v.screenpos = v.pos * 0.5f + 0.5f;
                v.screenpos.x *= X_size;
                v.screenpos.y *= Y_size;
            }
        }
    }

    /* view clipping
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
        float area = draw_util::cc_signed_area(triangle.vertices[0].screenpos, triangle.vertices[1].screenpos, triangle.vertices[2].screenpos);

        // iterate through pixels
        for (int x = min_x; x < max_x; ++x) {
            for (int y = min_y; y < max_y; ++y) {
                glm::vec2 p {x, y};

                /* calculate barycentric coordinates
                 * reference: https://ceng2.ktu.edu.tr/~cakir/files/grafikler/Texture_Mapping.pdf */
                float v = draw_util::cc_signed_area(p, triangle.vertices[1].screenpos, triangle.vertices[2].screenpos) / area;
                float w = draw_util::cc_signed_area(p, triangle.vertices[2].screenpos, triangle.vertices[0].screenpos) / area;
                float u = 1.0f - v - w;

                /* perspective-corrected barycentric coordinates
                 * reference: https://stackoverflow.com/questions/24441631/how-exactly-does-opengl-do-perspectively-correct-linear-interpolation */
                float b0 = v * triangle.vertices[0].pos.w;
                float b1 = w * triangle.vertices[1].pos.w;
                float b2 = u * triangle.vertices[2].pos.w;
                float inv_b_sum = 1.0f / (b0 + b1 + b2);
                b0 *= inv_b_sum;
                b1 *= inv_b_sum;
                b2 *= inv_b_sum;

                /* Not using barycentric coordinates for checking if inside
                 * triangle to avoid gaps.
                 * not this:  if (b0 >= 0 && b1 >= 0 && b2 >= 0)
                 * instead this: */
                if (draw_util::is_point_in_triangle(p, p0, p1, p2)) {
                    // interpolate depth
                    float z = b0 * triangle.vertices[0].pos.z
                            + b1 * triangle.vertices[1].pos.z
                            + b2 * triangle.vertices[2].pos.z;

                    // create fragment if depth test passes
                    #pragma omp critical
                    {
                        if(z < zbuf.buf[x][y]) {
                            frag_buf.buf[x][y] = fragment(&triangle, b0, b1, b2);
                            zbuf.buf[x][y] = z;
                        }
                    }
                }
            }
        }
    }
}

void Render::execute_fragment_shader(glm::vec3 (*frag_shader)(fragment, float)) {
    #pragma omp parallel for schedule(static) collapse(2)
    for (int x = 0; x < X_size; ++x) {
        for (int y = 0; y < Y_size; ++y) {
            if (frag_buf.buf[x][y].has_value()) {
                fbuf.buf[x][y] = frag_shader(frag_buf.buf[x][y].value(), global_time);
            }
        }
    }
}

void Render::draw_fbuf() {
    string printbuf = "";

    for (size_t y = 0; y < Y_size; y++) {
        if(y) printbuf.append("\n");
        for (size_t x = 0; x < X_size; x++) {
            printbuf.append(draw_util::ansi_color_string(draw_util::BG, fbuf.buf[x][y]));
            printbuf.append(" ");
            printbuf.append(draw_util::ansi_clear_string());
        }
    }

    printf("%s", printbuf.c_str());
    fflush(stdout);
}

} /* end of namespace tc */
