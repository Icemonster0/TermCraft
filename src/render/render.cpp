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

    // mesh m;
    // m.tri_list.emplace_back(vertex(-0.5, 0.5, 0.0), vertex(0.5, 0.5, 0.0), vertex(0.0, -0.5, 0.0));
    // m.tri_list[0].vertices[0].color = glm::vec3(1.0f, 0.0f, 0.0f);
    // m.tri_list[0].vertices[1].color = glm::vec3(0.0f, 1.0f, 0.0f);
    // m.tri_list[0].vertices[2].color = glm::vec3(0.0f, 0.0f, 1.0f);

    execute_vertex_shader(&m, vert_shaders::VERT_camera);
    rasterize(&m);
    execute_fragment_shader(frag_shaders::FRAG_default);

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

    for (int i = 0; i < m->tri_list.size(); ++i) {
        tri &triangle = m->tri_list[i];

        for (vertex &v : triangle.vertices) {
            // programmable shader
            vert_shader(&v, VP, global_time);

            // depth division
            v.pos = glm::vec4(v.pos.xyz() / v.pos.w, v.pos.w);
        }

        /* view clipping
         * If every vertex is outside of NDC space, the triangle
         * is deleted. */
        if (glm::any(glm::greaterThan(glm::abs(triangle.vertices[0].pos.xyz()), glm::vec3(1.0f))) &&
            glm::any(glm::greaterThan(glm::abs(triangle.vertices[1].pos.xyz()), glm::vec3(1.0f))) &&
            glm::any(glm::greaterThan(glm::abs(triangle.vertices[2].pos.xyz()), glm::vec3(1.0f)))) {

            m->tri_list.erase(m->tri_list.begin() + i);
            i--;

        } else {
            // screen transform
            for (vertex &v : triangle.vertices) {
                v.screenpos = v.pos * 0.5f + 0.5f;
                v.screenpos.x *= X_size;
                v.screenpos.y *= Y_size;
            }
        }
    }

    n_active_tris = m->tri_list.size(); // for debug info
}

void Render::rasterize(mesh *m) {
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

        /* pre-calculate things for the barycentric coordinates
         * method from: https://ceng2.ktu.edu.tr/~cakir/files/grafikler/Texture_Mapping.pdf */
        glm::vec2 v0 = triangle.vertices[1].screenpos.xy() - triangle.vertices[0].screenpos.xy();
        glm::vec2 v1 = triangle.vertices[2].screenpos.xy() - triangle.vertices[0].screenpos.xy();
        float d00 = glm::dot(v0, v0);
        float d01 = glm::dot(v0, v1);
        float d11 = glm::dot(v1, v1);
        float denom = d00 * d11 - d01 * d01;

        // iterate through pixels
        for (int x = min_x; x < max_x; ++x) {
            for (int y = min_y; y < max_y; ++y) {
                glm::vec2 p {x, y};

                /* calculate barycentric coordinates
                 * method from: see above */
                glm::vec2 v2 = p - triangle.vertices[0].screenpos.xy();
                float d20 = glm::dot(v2, v0);
                float d21 = glm::dot(v2, v1);
                float w0 = (d11 * d20 - d01 * d21) / denom;
                float w1 = (d00 * d21 - d01 * d20) / denom;
                float w2 = 1.0f - w0 - w1;

                if (w0 > 0 && w1 > 0 && w2 > 0) {
                    // interpolate depth
                    float z = w0 * triangle.vertices[0].pos.z
                            + w1 * triangle.vertices[1].pos.z
                            + w2 * triangle.vertices[2].pos.z;

                    // create fragment if depth test passes
                    if(z < zbuf.buf[x][y]) {
                        frag_buf.buf[x][y] = fragment(&triangle, w0, w1, w2);
                        zbuf.buf[x][y] = z;
                    }
                }
            }
        }
    }
}

void Render::execute_fragment_shader(glm::vec3 (*frag_shader)(fragment, float)) {
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
