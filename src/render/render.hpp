#ifndef RENDER_HPP
#define RENDER_HPP

#include "../glm.hpp"

#include "buffer.hpp"
#include "../misc/fragment.hpp"
#include "../misc/mesh.hpp"

#include <vector>

namespace tc {

class Render {
public:
    Render(int p_X_size, int p_Y_size);
    Render() {}

    void render();

private:
    void clear_buffers();
    void execute_vertex_shader(mesh *m, vertex (*vert_shader)(vertex));
    std::vector<fragment> rasterize(mesh m);
    void draw_fbuf();

    int X_size;
    int Y_size;

    buffer<glm::vec3> fbuf;
    buffer<float> zbuf;
};

} /* end of namespace tc */

#endif /* end of include guard: RENDER_HPP */
