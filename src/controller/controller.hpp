#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../glm.hpp"

#include "camera.hpp"
#include "input_state.hpp"
#include "../world/world.hpp"
#include "../world/block.hpp"
#include "../world/raycast_util.hpp"
#include "../user_settings.hpp"

#include <algorithm>
#include <optional>
#include <list>

namespace tc {

class Controller {
public:
    Controller(glm::vec3 p_pos, float p_aspect, float p_interact_range, World *p_world_ptr);
    Controller() {}

    glm::mat4 get_VP_matrix();
    glm::mat4 get_V_matrix();
    void input_event(char key);
    void simulation_step(float delta_time);
    void update_aspect(float value);
    void get_params(glm::vec3 *pos_ptr, glm::vec3 *velocity_ptr, glm::vec2 *look_ptr);
    bool is_flying();
    bool is_sprinting();
    bool is_crouching();
    block_type::Block_Type get_active_block_type();

private:
    void simulate(float delta_time);
    void register_input_keys();
    void evaluate_misc_inputs(float delta_time);
    void move(glm::vec3 dir);
    void turn(glm::vec2 dir);
    void calc_looked_at_block(bool adjacent);
    void toggle_fly();
    void toggle_crouch();
    void toggle_sprint();
    bool is_block_solid(glm::vec3 sample_point);

    glm::vec3 pos;
    glm::vec3 old_pos;
    glm::vec3 velocity;
    bool flying;
    bool crouching;
    bool sprinting;
    bool is_on_ground;
    float height;
    float interact_range;

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
