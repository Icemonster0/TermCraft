#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../glm.hpp"

#include "camera.hpp"
#include "input_state.hpp"
#include "../world/world.hpp"
#include "../world/block.hpp"
#include "../user_settings.hpp"

#include <algorithm>
#include <optional>

namespace tc {

class Controller {
public:
    Controller(glm::vec3 p_pos, float p_aspect, float p_height, float p_interact_range, float p_move_speed, float p_look_sensitivity, World *p_world_ptr);
    Controller() {}

    glm::mat4 get_VP_matrix();
    glm::mat4 get_V_matrix();
    void input_event(char key);
    void simulation_step(float delta_time);
    void update_aspect(float value);
    void get_params(glm::vec3 *pos_ptr, glm::vec2 *look_ptr);
    block_type::Block_Type get_active_block_type();

private:
    void register_input_keys();
    void evaluate_inputs(float delta_time);
    void move(glm::vec3 dir);
    void turn(glm::vec2 dir);
    void calc_looked_at_block(bool adjacent);

    glm::vec3 pos;
    glm::vec3 old_pos;
    float height;
    float interact_range;
    float move_speed;
    float look_sensitivity;
    float render_distance;

    block_type::Block_Type active_block_type;
    std::optional<glm::ivec3> looked_at_block;
    std::optional<glm::ivec3> old_looked_at_block;
    glm::ivec2 this_chunk_coord {0};

    Camera camera;
    Input_State input_state;
    World *world_ptr;
};

} /* end of namespace tc */

#endif /* end of include guard: CONTROLLER_H */
