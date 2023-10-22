#include "../glm.hpp"

#include "controller.hpp"
#include "camera.hpp"
#include "input_state.hpp"

#include <algorithm>

namespace tc {

// public:

Controller::Controller(glm::vec3 p_pos, float p_aspect, float p_height, float p_move_speed, float p_look_sensitivity)
 : pos(p_pos), move_speed(p_move_speed), look_sensitivity(p_look_sensitivity), height(p_height) {
    camera = Camera {45.0f, p_aspect, 0.01f, 100.0f, p_pos};
    input_state = Input_State {};

    register_input_keys();
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
    input_state.add_key('w');
    input_state.add_key('a');
    input_state.add_key('s');
    input_state.add_key('d');
    input_state.add_key(' ');
    input_state.add_key('c');
    input_state.add_key('8');
    input_state.add_key('2');
    input_state.add_key('4');
    input_state.add_key('6');
}

void Controller::evaluate_inputs(float delta_time) {
    if(input_state.get_key('w'))
        move(camera.get_forward_vector() * move_speed * delta_time);

    if(input_state.get_key('a'))
        move(-camera.get_right_vector() * move_speed * delta_time);

    if(input_state.get_key('s'))
        move(-camera.get_forward_vector() * move_speed * delta_time);

    if(input_state.get_key('d'))
        move(camera.get_right_vector() * move_speed * delta_time);

    if(input_state.get_key(' '))
        move(glm::vec3(0.0f, -1.0f, 0.0f) * move_speed * delta_time);

    if(input_state.get_key('c'))
        move(glm::vec3(0.0f, 1.0f, 0.0f) * move_speed * delta_time);

    if(input_state.get_key('8'))
        turn(glm::vec2(0.0f, look_sensitivity * delta_time));

    if(input_state.get_key('2'))
        turn(glm::vec2(0.0f, -look_sensitivity * delta_time));

    if(input_state.get_key('4'))
        turn(glm::vec2(look_sensitivity * delta_time, 0.0f));

    if(input_state.get_key('6'))
        turn(glm::vec2(-look_sensitivity * delta_time, 0.0f));
}

void Controller::move(glm::vec3 dir) {
    pos += dir;
    camera.pos = pos + glm::vec3(0.0f, -height, 0.0f);

    camera.calc_V_matrix();
    camera.calc_VP_matrix();
}

void Controller::turn(glm::vec2 dir) {
    camera.yaw += dir.x;
    camera.pitch = std::clamp(camera.pitch + dir.y, -90.0f, 90.0f);

    camera.calc_V_matrix();
    camera.calc_VP_matrix();
}

} /* end of namespace tc */
