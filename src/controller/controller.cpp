#include "../glm.hpp"

#include "controller.hpp"
#include "camera.hpp"
#include "input_state.hpp"
#include "../world/world.hpp"
#include "../world/block.hpp"

#include <algorithm>
#include <optional>

namespace tc {

// public:

Controller::Controller(glm::vec3 p_pos, float p_aspect, float p_height, float p_interact_range, float p_move_speed, float p_look_sensitivity, World *p_world_ptr)
 : pos(p_pos), height(p_height), interact_range(p_interact_range), move_speed(p_move_speed), look_sensitivity(p_look_sensitivity), world_ptr(p_world_ptr), active_block_type(block_type::GRASS) {

    camera = Camera {45.0f, p_aspect, 0.01f, 100.0f, p_pos + glm::vec3(0, -height, 0)};
    input_state = Input_State {};

    register_input_keys();
    calc_looked_at_block(false);
}

glm::mat4 Controller::get_VP_matrix() {
    return camera.get_VP_matrix();
}

void Controller::input_event(char key) {
    input_state.activate_key(key);
}

void Controller::simulation_step(float delta_time) {
    evaluate_inputs(delta_time);
    input_state.time_step(delta_time);

    world_ptr->highlight_block(looked_at_block.has_value() ? looked_at_block.value() : glm::ivec3(-1));
}

void Controller::update_aspect(float value) {
    camera.aspect = value;

    camera.calc_P_matrix();
    camera.calc_VP_matrix();
}

void Controller::get_params(glm::vec3 *pos_ptr, glm::vec2 *look_ptr) {
    *pos_ptr = pos;
    *look_ptr = glm::vec2(camera.yaw, camera.pitch);
}

// private:

void Controller::register_input_keys() {
    // movement
    input_state.add_key('w');
    input_state.add_key('a');
    input_state.add_key('s');
    input_state.add_key('d');

    // fly / jump / crouch
    input_state.add_key(' ');
    input_state.add_key('c');

    // look
    input_state.add_key('8');
    input_state.add_key('2');
    input_state.add_key('4');
    input_state.add_key('6');

    // alt look
    input_state.add_key('i');
    input_state.add_key('k');
    input_state.add_key('j');
    input_state.add_key('l');

    // interact
    input_state.add_single_event_key('e');
    input_state.add_single_event_key('f');
}

void Controller::evaluate_inputs(float delta_time) {
    // movement

    if(input_state.get_key('w'))
        move(camera.get_h_forward_vector() * move_speed * delta_time);
    if(input_state.get_key('a'))
        move(-camera.get_right_vector() * move_speed * delta_time);
    if(input_state.get_key('s'))
        move(-camera.get_h_forward_vector() * move_speed * delta_time);
    if(input_state.get_key('d'))
        move(camera.get_right_vector() * move_speed * delta_time);

    // fly / jump / crouch

    if(input_state.get_key(' '))
        move(glm::vec3(0.0f, -1.0f, 0.0f) * move_speed * delta_time);

    if(input_state.get_key('c'))
        move(glm::vec3(0.0f, 1.0f, 0.0f) * move_speed * delta_time);

    // look

    if(input_state.get_key('8') || input_state.get_key('i'))
        turn(glm::vec2(0.0f, look_sensitivity * delta_time));
    if(input_state.get_key('2') || input_state.get_key('k'))
        turn(glm::vec2(0.0f, -look_sensitivity * delta_time));
    if(input_state.get_key('4') || input_state.get_key('j'))
        turn(glm::vec2(look_sensitivity * delta_time, 0.0f));
    if(input_state.get_key('6') || input_state.get_key('l'))
        turn(glm::vec2(-look_sensitivity * delta_time, 0.0f));

    // interact

    if(input_state.get_key('e')) {
        if (looked_at_block.has_value())
            world_ptr->replace(looked_at_block.value(), block_type::EMPTY);

        calc_looked_at_block(false);
    }

    if(input_state.get_key('f')) {
        calc_looked_at_block(true);

        if (looked_at_block.has_value())
            world_ptr->replace(looked_at_block.value(), active_block_type);

        calc_looked_at_block(false);
    }
}

void Controller::move(glm::vec3 dir) {
    pos += dir;
    camera.pos = pos + glm::vec3(0.0f, -height, 0.0f);

    camera.calc_V_matrix();
    camera.calc_VP_matrix();

    calc_looked_at_block(false);
}

void Controller::turn(glm::vec2 dir) {
    camera.yaw += dir.x;
    camera.pitch = std::clamp(camera.pitch + dir.y, -90.0f, 90.0f);

    camera.calc_V_matrix();
    camera.calc_VP_matrix();

    calc_looked_at_block(false);
}

void Controller::calc_looked_at_block(bool adjacent) {
    glm::vec3 dir = camera.get_forward_vector();
    float step = 0.1f;

    for (float f = 0.0f; f <= interact_range; f += step) {
        if (world_ptr->get_block(camera.pos + f * dir)->type != block_type::EMPTY) {
            if (adjacent) {
                looked_at_block = std::optional<glm::ivec3> {camera.pos + (f - step) * dir};
                return;
            } else {
                looked_at_block = std::optional<glm::ivec3> {camera.pos + f * dir};
                return;
            }
        }
    }

    looked_at_block = std::optional<glm::ivec3> {};
}

} /* end of namespace tc */
