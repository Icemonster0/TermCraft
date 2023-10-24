#include "../glm.hpp"

#include "camera.hpp"

namespace tc {

Camera::Camera(float p_fov, float p_aspect, float p_near, float p_far, glm::vec3 p_pos)
 : fov(p_fov), aspect(p_aspect), near(p_near), far(p_far), pos(p_pos), yaw(0.0f), pitch(0.0f) {
     calc_V_matrix();
     calc_P_matrix();
     calc_VP_matrix();
}

glm::mat4 Camera::get_VP_matrix() {
    return VP;
}

glm::vec3 Camera::get_h_forward_vector() {
    // returns the horizontal forward vector (not affected by pitch)
    return glm::rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(yaw), glm::vec3(0.0f, -1.0f, 0.0f));
}

glm::vec3 Camera::get_forward_vector() {
    // returns the true forward vector (affected by pitch)
    return glm::rotate(
        glm::rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f)),
        glm::radians(yaw),
        glm::vec3(0.0f, -1.0f, 0.0f)
    );
}

glm::vec3 Camera::get_right_vector() {
    return glm::rotate(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(yaw), glm::vec3(0.0f, -1.0f, 0.0f));
}

void Camera::calc_V_matrix() {
    V = glm::mat4(1.0f);
    V = glm::rotate(V, glm::radians(-pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    V = glm::rotate(V, glm::radians(-yaw), glm::vec3(0.0f, -1.0f, 0.0f));
    V = glm::translate(V, -pos);
}

void Camera::calc_P_matrix() {
    P = glm::mat4(1.0f);
    P = glm::perspective(fov, aspect / 2.0f, near, far);
}

void Camera::calc_VP_matrix() {
    VP = P * V;
}

} /* end of namespace tc */
