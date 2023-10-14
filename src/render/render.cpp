#include "../glm.hpp"

#include "render.hpp"
#include "buffer.hpp"
#include "draw_util.hpp"
#include "../misc/mesh.hpp"
#include "../shaders/vert_shaders.hpp"

#include <string>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <array>

using namespace std;

namespace tc {

Render::Render(int p_X_size, int p_Y_size) : X_size(p_X_size), Y_size(p_Y_size) {
    clear_buffers();
}

void Render::render(float time) {
    global_time = time;

    clear_buffers();

    mesh m;
    m.tri_list.emplace_back(vertex(-0.5, 0.5, 0.0), vertex(0.5, 0.5, 0.0), vertex(0.0, -0.5, 0.0));

    execute_vertex_shader(&m, vert_shaders::VERT_fun);
    vector<fragment> frag_list = rasterize(m);
    // printf("%ld\n", frag_list.size());
    execute_fragment_shader(&fbuf, frag_list);

    draw_fbuf();
    printf("%f\n", global_time);
}

void Render::clear_buffers() {
    fbuf.clear(X_size, Y_size, glm::vec3(0.0f));
    zbuf.clear(X_size, Y_size, 0.0f);
}

void Render::execute_vertex_shader(mesh *m, vertex (*vert_shader)(vertex, float)) {
    for (tri &triangle : m->tri_list) {
        for (vertex &v : triangle.vertices) {
            v = vert_shader(v, global_time);

            // screen_transform
            v.pos = v.pos * 0.5f + 0.5f;
            v.pos.x *= X_size;
            v.pos.y *= Y_size;
        }
    }
}

vector<fragment> Render::rasterize(mesh m) {
    vector<fragment> frag_list;

    for (tri triangle : m.tri_list) {
        // if (triangle.vertices[0].pos.y != triangle.vertices[1].pos.y &&
        //     triangle.vertices[0].pos.y != triangle.vertices[2].pos.y &&
        //     triangle.vertices[1].pos.y != triangle.vertices[2].pos.y) {

            array<glm::vec2, 3> points = {triangle.vertices[0].pos.xy(), triangle.vertices[1].pos.xy(), triangle.vertices[2].pos.xy()};
            sort(points.begin(), points.end(), [](glm::vec2 a, glm::vec2 b) {return a.y < b.y;});
            // printf("%f %f %f %f %f %f\n", points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y);

            // TODO: only flat-bottom triangle supported

            float inv_slope_1 = (points[1].x - points[0].x) / (points[1].y - points[0].y);
            float inv_slope_2 = (points[2].x - points[0].x) / (points[2].y - points[0].y);
            // printf("%f %f\n", inv_slope_1, inv_slope_2);

            float x_1 = points[0].x;
            float x_2 = points[0].x;

            for (int y = points[0].y; y < points[2].y; y++) {
                // TODO: optimize, depth checking
                if (x_1 < x_2) {
                    for (float x = x_1; x < x_2; x++) {
                        if (x >= 0 && x < X_size && y >= 0 && y < Y_size)
                            frag_list.emplace_back(static_cast<int>(x), y);
                    }
                } else {
                    for (float x = x_2; x > x_1; x--) {
                        if (x >= 0 && x < X_size && y >= 0 && y < Y_size)
                            frag_list.emplace_back(static_cast<int>(x), y);
                    }
                }
                x_1 += inv_slope_1;
                x_2 += inv_slope_2;
            }
        // }
    }

    return frag_list;
}

void Render::execute_fragment_shader(buffer<glm::vec3> *write_buf, vector<fragment> frag_list) {
    for (fragment frag : frag_list) {
        write_buf->buf[frag.pos.x][frag.pos.y] = glm::vec3(1.0f);
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

    system("tput cup 0 0");
    printf("%s", printbuf.c_str());
    fflush(stdout);
}

} /* end of namespace tc */
