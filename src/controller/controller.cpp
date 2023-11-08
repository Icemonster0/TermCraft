#include "../glm.hpp"

#include "controller.hpp"
#include "camera.hpp"
#include "input_state.hpp"
#include "../world/world.hpp"
#include "../world/block.hpp"
#include "../user_settings.hpp"

#include <algorithm>
#include <optional>

namespace tc {

// public:

Controller::Controller(glm::vec3 p_pos,
                       float p_aspect,
                       float p_height,
                       float p_interact_range,
                       float p_move_speed,
                       float p_look_sensitivity,
                       World *p_world_ptr) :
                       pos(p_pos),
                       old_pos({-1.0f}),
                       height(p_height),
                       interact_range(p_interact_range),
                       move_speed(p_move_speed),
                       look_sensitivity(p_look_sensitivity),
                       world_ptr(p_world_ptr),
                       active_block_type(block_type::GRASS) {

    camera = Camera {U.fov, // fov
                     p_aspect, // aspect ratio
                     0.01f, // near plane
                     p_pos + glm::vec3(0, -height, 0)}; // position

    input_state = Input_State {};

    register_input_keys();
    calc_looked_at_block(false);
    world_ptr->update_chunks(pos, old_pos, U.render_distance);
}

glm::mat4 Controller::get_VP_matrix() {
    return camera.get_VP_matrix();
}

glm::mat4 Controller::get_V_matrix() {
    return camera.get_V_matrix();
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
    old_pos = pos;
    pos += dir;
    camera.pos = pos + glm::vec3(0.0f, -height, 0.0f);

    camera.calc_V_matrix();
    camera.calc_VP_matrix();

    calc_looked_at_block(false);
    world_ptr->update_chunks(pos, old_pos, U.render_distance);
}

void Controller::turn(glm::vec2 dir) {
    camera.yaw += dir.x;
    camera.pitch = std::clamp(camera.pitch + dir.y, -90.0f, 90.0f);

    camera.calc_V_matrix();
    camera.calc_VP_matrix();

    calc_looked_at_block(false);
}

void Controller::calc_looked_at_block(bool adjacent) {
    const glm::vec3 dir = camera.get_forward_vector();
    const glm::vec3 start = camera.pos;
    const glm::vec3 end = start + dir * interact_range;

    glm::vec3 diff = end - start;
    float interact_range_squared = interact_range * interact_range;

    glm::ivec3 block = start;
    const glm::ivec3 dir_sign = glm::sign(dir);
    glm::ivec3 block_step {0};

    glm::ivec3 next_border {};

    while (true) {
        if (world_ptr->get_block(block)->type != block_type::EMPTY) {
            if (adjacent) {
                looked_at_block = std::optional<glm::ivec3> {block - block_step};
                return;
            } else {
                looked_at_block = std::optional<glm::ivec3> {block};
                return;
            }
        }

        glm::bvec3 is_block_negative = glm::lessThan(glm::sign(block), glm::ivec3(0));
        next_border.x = block.x + std::clamp(dir_sign.x, 0 - is_block_negative.x, 1 - is_block_negative.x);
        next_border.y = block.y + std::clamp(dir_sign.y, 0 - is_block_negative.y, 1 - is_block_negative.y);
        next_border.z = block.z + std::clamp(dir_sign.z, 0 - is_block_negative.z, 1 - is_block_negative.z);

        float intersect_x_fac = (next_border.x - start.x) / (end.x - start.x);
        glm::ivec2 intersect_x {
            start.y + (end.y - start.y) * intersect_x_fac,
            start.z + (end.z - start.z) * intersect_x_fac
        };
        float intersect_y_fac = (next_border.y - start.y) / (end.y - start.y);
        glm::ivec2 intersect_y {
            start.x + (end.x - start.x) * intersect_y_fac,
            start.z + (end.z - start.z) * intersect_y_fac
        };
        float intersect_z_fac = (next_border.z - start.z) / (end.z - start.z);
        glm::ivec2 intersect_z {
            start.x + (end.x - start.x) * intersect_z_fac,
            start.y + (end.y - start.y) * intersect_z_fac
        };

        if (intersect_x == block.yz()) {
            block_step = glm::ivec3 {1, 0, 0} * dir_sign;
        } else if (intersect_y == block.xz()) {
            block_step = glm::ivec3 {0, 1, 0} * dir_sign;
        } else if (intersect_z == block.xy()) {
            block_step = glm::ivec3 {0, 0, 1} * dir_sign;
        }

        block += block_step;

        glm::vec3 diff = glm::vec3(block) - start;
        if ((diff.x*diff.x + diff.y*diff.y + diff.z*diff.z) > interact_range_squared) {
            break;
        }
    }

    looked_at_block = std::optional<glm::ivec3> {};
}

} /* end of namespace tc */
