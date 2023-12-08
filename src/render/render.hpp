#ifndef RENDER_HPP
#define RENDER_HPP

#include "../glm.hpp"

#include "buffer.hpp"
#include "fragment.hpp"
#include "mesh.hpp"
#include "../world/block.hpp"
#include "draw_util.hpp"
#include "../shaders/vert_shaders.hpp"
#include "../shaders/frag_shaders.hpp"
#include "../shaders/post_shaders.hpp"
#include "../user_settings.hpp"

#include <string>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <type_traits>
#include <iterator>
#include <vector>
#include <optional>
#include <list>

namespace tc {

class Render {
public:
    Render(int p_X_size, int p_Y_size);
    Render() {}

    void render(mesh m);
    void set_debug_info(std::string debug_info);
    void set_params(int p_X_size, int p_Y_size, float p_global_time, float p_time_of_day, glm::mat4 p_V, glm::mat4 p_VP, block_type::Block_Type p_active_block_type, bool p_flying, bool p_crouching, bool p_sprinting);
    void get_params(int *n_tris_ptr, int *n_active_tris_ptr);

private:
    void time_of_day_update();
    void clear_buffers();
    void execute_vertex_shader(mesh *m, void (*vert_shader)(vertex*, glm::mat4, glm::mat4, float));
    void rasterize(mesh *m);
    void execute_fragment_and_post_shaders(glm::vec3 (*frag_shader)(fragment, glm::vec3, float, float),
                                           glm::vec3 (*post_shader)(const buffer<glm::vec3>*, glm::ivec2, glm::ivec2, float));
    void construct_hud();
    void draw_fbuf();

    int X_size;
    int Y_size;
    float global_time = 0.0f;
    float time_of_day = 0.0f;
    glm::vec3 sun_direction {1.0f};
    float sky_brightness = 1.0f;
    glm::mat4 VP = glm::mat4 {};
    glm::mat4 V = glm::mat4 {};
    block_type::Block_Type active_block_type {};

    bool flying, crouching, sprinting;

    int n_tris = 0;
    int n_active_tris = 0;

    buffer<glm::vec3> fbuf;
    buffer<std::list<fragment>> frag_buf;
    buffer<std::string> hud_buf;
    buffer<char> debug_buf;
};

} /* end of namespace tc */

#endif /* end of include guard: RENDER_HPP */
