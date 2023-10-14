#include "../glm.hpp"

#include "render.hpp"
#include "buffer.hpp"
#include "draw_util.hpp"
#include "../misc/mesh.hpp"

#include <string>
#include <cstdio>
#include <cstdlib>

using namespace std;

namespace tc {

Render::Render(int p_X_size, int p_Y_size) : X_size(p_X_size), Y_size(p_Y_size) {
    clear_buffers();
}

void Render::render() {
    clear_buffers();

    mesh m;
    m.tri_list.emplace_back(vertex(-0.5, -0.5, 0.0), vertex(0.5, -0.5, 0.0), vertex(0.0, 0.5, 0.0));


    screen_transform(&m);
    vector<fragment> frag_list = rasterize(m);

    draw_fbuf();
}

void Render::clear_buffers() {
    fbuf.clear(X_size, Y_size, glm::vec3(0.0f));
    zbuf.clear(X_size, Y_size, 0.0f);
}

void Render::execute_vertex_shader(mesh *m) {
    for (auto &triangle : m.tri_list) {
        for (vertex &v : triangle.vertices) {
            // TODO
        }
    }
}

void Render::screen_transform(mesh *m) {
    for (auto &triangle : m.tri_list) {
}

vector<fragment> Render::rasterize(mesh m) {
    vector<fragment> frag_list;

    for (auto triangle : m.tri_list) {
        float
        for (int y = )
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
