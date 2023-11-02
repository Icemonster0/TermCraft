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

    void render(mesh m);
    void set_debug_info(std::string debug_info);
    void set_params(int p_X_size, int p_Y_size, float p_global_time, glm::mat4 p_V, glm::mat4 p_VP);
    void get_params(int *n_tris_ptr, int *n_active_tris_ptr);

private:
    void clear_buffers();
    void execute_vertex_shader(mesh *m, void (*vert_shader)(vertex*, glm::mat4, glm::mat4, float));
    void rasterize(mesh *m);
    void execute_fragment_shader(glm::vec3 (*frag_shader)(fragment, float));
    void draw_fbuf();

    int X_size;
    int Y_size;
    float global_time = 0.0f;
    glm::mat4 VP = glm::mat4 {};
    glm::mat4 V = glm::mat4 {};

    int n_tris = 0;
    int n_active_tris = 0;

    buffer<glm::vec3> fbuf;
    buffer<float> zbuf;
    buffer<std::optional<fragment>> frag_buf;
    buffer<std::string> hud_buf;
    buffer<char> debug_buf;
};

} /* end of namespace tc */

#endif /* end of include guard: RENDER_HPP */
