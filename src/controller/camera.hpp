#ifndef CAMERA_H
#define CAMERA_H

#include "../glm.hpp"

#include "../user_settings.hpp"

namespace tc {

class Camera {
public:
    Camera(float p_fov, float p_aspect, float p_near, glm::vec3 p_pos);
    Camera() {}

    glm::mat4 get_VP_matrix();
    glm::mat4 get_V_matrix();
    glm::mat4 get_P_matrix();
    glm::vec3 get_h_forward_vector();
    glm::vec3 get_forward_vector();
    glm::vec3 get_right_vector();

    void calc_V_matrix();
    void calc_P_matrix();
    void calc_VP_matrix();

    glm::vec3 pos;
    float pitch;
    float yaw;
    float fov;
    float aspect;
    float near;

private:

    glm::mat4 V {1.0f};
    glm::mat4 P {1.0f};
    glm::mat4 VP {1.0f};
};

} /* end of namespace tc */

#endif /* end of include guard: CAMERA_H */
