#include "../glm.hpp"

#include "controller.hpp"
#include "camera.hpp"

#include <algorithm>

Controller::Controller(float p_aspect, float p_height, float p_move_speed, float p_look_sensitivity)
 : move_speed(p_move_speed), look_sensitivity(p_look_sensitivity), height(p_height) {
    camera = Camera {45.0f, p_aspect, 0.1f, 100.0f};
}

glm::mat4 Controller::get_VP_matrix() {
    return camera.get_VP_matrix();
}

void Controller::input_event(char key) {
    switch (key) {
        case 'w': move(camera.get_forward_vector()); break;
        case 'a': move(-camera.get_right_vector()); break;
        case 's': move(-camera.get_forward_vector()); break;
        case 'd': move(camera.get_right_vector()); break;
        case '8': turn(glm::vec2(0.0f, 1.0f)); break;
        case '2': turn(glm::vec2(0.0f, -1.0f)); break;
        case '4': turn(glm::vec2(1.0f, 0.0f)); break;
        case '6': turn(glm::vec2(-1.0f, 0.0f)); break;
        default: break;
    }
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

void Controller::move(glm::vec3 dir) {
    pos += dir * move_speed;
    camera.pos = pos + glm::vec3(0.0f, height, 0.0f);

    camera.calc_V_matrix();
    camera.calc_VP_matrix();
}

void Controller::turn(glm::vec2 dir) {
    camera.yaw += dir.x * look_sensitivity;
    camera.pitch = std::clamp(camera.pitch + dir.y * look_sensitivity, -90.0f, 90.0f);

    camera.calc_V_matrix();
    camera.calc_VP_matrix();
}
