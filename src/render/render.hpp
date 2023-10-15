#ifndef RENDER_HPP
#define RENDER_HPP

#include "../glm.hpp"

#include "buffer.hpp"
#include "fragment.hpp"
#include "mesh.hpp"

#include <vector>
#include <optional>

namespace tc {

class Render {
public:
    Render(int p_X_size, int p_Y_size);
    Render() {}

    void render();
    void set_params(int p_X_size, int p_Y_size, float p_global_time);

private:
    void clear_buffers();
    void execute_vertex_shader(mesh *m, void (*vert_shader)(vertex*, float));
    void rasterize(mesh *m);
    void execute_fragment_shader(glm::vec3 (*frag_shader)(fragment, float));
    void draw_fbuf();

    int X_size;
    int Y_size;
    float global_time;

    buffer<glm::vec3> fbuf;
    buffer<float> zbuf;
    buffer<std::optional<fragment>> frag_buf;
};

} /* end of namespace tc */

#endif /* end of include guard: RENDER_HPP */
