#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../glm.hpp"

#include "camera.hpp"
#include "input_state.hpp"

namespace tc {

class Controller {
public:
    Controller(glm::vec3 p_pos, float p_aspect, float p_height, float p_move_speed, float p_look_sensitivity);
    Controller() {}

    glm::mat4 get_VP_matrix();
    void input_event(char key);
    void simulation_step(float delta_time);
    void update_aspect(float value);
    void get_params(glm::vec3 *pos_ptr, glm::vec2 *look_ptr);

private:
    void add_input_keys();
    void evaluate_inputs(float delta_time);
    void move(glm::vec3 dir);
    void turn(glm::vec2 dir);

    glm::vec3 pos;
    float height;
    float move_speed;
    float look_sensitivity;

    Camera camera;
    Input_State input_state;
};

} /* end of namespace tc */

#endif /* end of include guard: CONTROLLER_H */
