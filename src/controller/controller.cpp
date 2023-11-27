#include "controller.hpp"

namespace tc {

#define GRAVITY 9.81f
#define ACCELERATION 30.0f
#define AIR_ACCELERATION 1.0f
#define BRAKE_FACTOR 10.0f
#define AIR_BRAKE_FACTOR 0.1f
#define MAX_NORMAL_SPEED 3.0f
#define MAX_SPRINT_SPEED 10.0f
#define MAX_CROUCH_SPEED 1.0f
#define JUMP_STRENGTH 5.0f
#define TERMINAL_FALLING_VELOCITY 50.0f
#define NORMAL_HEIGHT 1.62f
#define CROUCH_HEIGHT 1.50f
#define VELOCITY_TERMINATOR 0.1f

// public:

Controller::Controller(glm::vec3 p_pos,
                       float p_aspect,
                       float p_interact_range,
                       World *p_world_ptr) :
                       pos(p_pos),
                       old_pos({-1.0f}),
                       velocity({0.0f}),
                       flying(true),
                       crouching(false),
                       sprinting(false),
                       is_on_ground(false),
                       height(NORMAL_HEIGHT),
                       interact_range(p_interact_range),
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
    old_looked_at_block = looked_at_block;
    old_pos = pos;

    simulate(delta_time);
    evaluate_misc_inputs(delta_time);

    input_state.time_step(delta_time);

    if (looked_at_block != old_looked_at_block) {
        world_ptr->highlight_block(looked_at_block.has_value() ? looked_at_block.value() : glm::ivec3(-1));
    }
}

void Controller::update_aspect(float value) {
    camera.aspect = value;

    camera.calc_P_matrix();
    camera.calc_VP_matrix();
}

void Controller::get_params(glm::vec3 *pos_ptr, glm::vec3 *velocity_ptr, glm::vec2 *look_ptr) {
    *pos_ptr = pos;
    *velocity_ptr = velocity;
    *look_ptr = glm::vec2(camera.yaw, camera.pitch);
}

block_type::Block_Type Controller::get_active_block_type() {
    return active_block_type;
}

// private:

void Controller::simulate(float delta_time) {
    glm::vec2 dir {
        (input_state.get_key('d') ? 1.0f : 0.0f) - (input_state.get_key('a') ? 1.0f : 0.0f),
        (input_state.get_key('w') ? 1.0f : 0.0f) - (input_state.get_key('s') ? 1.0f : 0.0f)
    };
    bool moving = glm::any(glm::notEqual(dir, glm::vec2(0.0f)));

    if (moving) dir = glm::normalize(dir);
    dir = dir.x * camera.get_right_vector().xz() + dir.y * camera.get_h_forward_vector().xz();

    float vertical_dir = (input_state.get_key('c') ? 1.0f : 0.0f) - (input_state.get_key(' ') ? 1.0f : 0.0f);
    bool vertically_moving = vertical_dir != 0.0f;

    if (!flying) {
        if (input_state.get_key('c')) {
            toggle_crouch();
        }

        if (is_block_solid(pos)) {
            if (moving) {
                velocity += glm::vec3(dir.x, 0.0f, dir.y) * ACCELERATION * delta_time;
            } else {
                velocity.x = glm::mix(velocity.x, 0.0f, BRAKE_FACTOR * delta_time);
                velocity.z = glm::mix(velocity.z, 0.0f, BRAKE_FACTOR * delta_time);
            }

            if (input_state.get_key(' ')) {
                velocity.y = -JUMP_STRENGTH * (crouching ? 0.5f : 1.0f);
                is_on_ground = false;
            } else {
                velocity.y = 0.0f;
                is_on_ground = true;
            }
        }
        else {
            if (moving) {
                velocity += glm::vec3(dir.x, 0.0f, dir.y) * AIR_ACCELERATION * delta_time;
            } else {
                velocity.x = glm::mix(velocity.x, 0.0f, AIR_BRAKE_FACTOR * delta_time);
                velocity.z = glm::mix(velocity.z, 0.0f, AIR_BRAKE_FACTOR * delta_time);
            }

            velocity.y += GRAVITY * delta_time;
        }
    }
    else { // if (flying)
        if (moving) {
            velocity += glm::vec3(dir.x, 0.0f, dir.y) * ACCELERATION * delta_time;
        } else {
            velocity.x = glm::mix(velocity.x, 0.0f, BRAKE_FACTOR * delta_time);
            velocity.z = glm::mix(velocity.z, 0.0f, BRAKE_FACTOR * delta_time);
        }
        if (vertically_moving) {
            velocity.y += vertical_dir * ACCELERATION * delta_time;
        } else {
            velocity.y = glm::mix(velocity.y, 0.0f, BRAKE_FACTOR * delta_time);
        }
    }

    const float max_horizontal_speed = sprinting ? MAX_SPRINT_SPEED : (crouching ? MAX_CROUCH_SPEED : MAX_NORMAL_SPEED);
    if (velocity.x*velocity.x + velocity.z*velocity.z > max_horizontal_speed*max_horizontal_speed) {
        glm::vec2 new_velocity = glm::normalize(velocity.xz()) * max_horizontal_speed;
        velocity = glm::vec3(new_velocity.x, velocity.y, new_velocity.y);
    }

    const float max_vertical_speed = flying ? max_horizontal_speed : TERMINAL_FALLING_VELOCITY;
    velocity.y = glm::clamp(velocity.y, -max_vertical_speed, max_vertical_speed);

    if (glm::all(glm::lessThan(glm::abs(velocity), glm::vec3(VELOCITY_TERMINATOR))))
        velocity = glm::vec3(0.0f);

    move(velocity * delta_time);
}

void Controller::register_input_keys() {
    // movement
    input_state.add_key('w');
    input_state.add_key('a');
    input_state.add_key('s');
    input_state.add_key('d');

    // fly / jump / walk / crouch / sprint
    input_state.add_key(' ');
    input_state.add_key('c');
    input_state.add_single_event_key('x');
    input_state.add_single_event_key('p');

    // look
    input_state.add_key('i');
    input_state.add_key('k');
    input_state.add_key('j');
    input_state.add_key('l');

    // interact
    input_state.add_single_event_key('e');
    input_state.add_single_event_key('f');

    // select active block
    input_state.add_single_event_key('1');
    input_state.add_single_event_key('2');
    input_state.add_single_event_key('3');
    input_state.add_single_event_key('4');
    input_state.add_single_event_key('5');
    input_state.add_single_event_key('6');
    input_state.add_single_event_key('7');
    input_state.add_single_event_key('8');
    input_state.add_single_event_key('9');
}

void Controller::evaluate_misc_inputs(float delta_time) {

    // fly / walk, sprint / normal

    if (input_state.get_key('x'))
        toggle_fly();

    if (input_state.get_key('p'))
        toggle_sprint();

    // look

    if (input_state.get_key('i'))
        turn(glm::vec2(0.0f, U.look_sensitivity * delta_time));
    if (input_state.get_key('k'))
        turn(glm::vec2(0.0f, -U.look_sensitivity * delta_time));
    if (input_state.get_key('j'))
        turn(glm::vec2(U.look_sensitivity * delta_time, 0.0f));
    if (input_state.get_key('l'))
        turn(glm::vec2(-U.look_sensitivity * delta_time, 0.0f));

    // interact

    if (input_state.get_key('e')) {
        if (looked_at_block.has_value())
            world_ptr->replace(looked_at_block.value(), block_type::EMPTY);

        calc_looked_at_block(false);
    }

    if (input_state.get_key('f') && active_block_type != block_type::EMPTY) {
        calc_looked_at_block(true);

        if (looked_at_block.has_value())
            world_ptr->replace(looked_at_block.value(), active_block_type);
    }

    // select active block

    if (input_state.get_key('1'))
        active_block_type = block_type::GRASS;
    if (input_state.get_key('2'))
        active_block_type = block_type::DIRT;
    if (input_state.get_key('3'))
        active_block_type = block_type::STONE;
    if (input_state.get_key('4'))
        active_block_type = block_type::OAK_LOG;
    if (input_state.get_key('5'))
        active_block_type = block_type::OAK_PLANKS;
    if (input_state.get_key('6'))
        active_block_type = block_type::FLOWER;
    if (input_state.get_key('7'))
        active_block_type = block_type::TUX;
    if (input_state.get_key('8'))
        active_block_type = block_type::EMPTY;
    if (input_state.get_key('9'))
        active_block_type = block_type::EMPTY;
}

void Controller::move(glm::vec3 dir) {
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

    for (int i = 0; i < 100; ++i) {
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

void Controller::toggle_fly() {
    if (flying) {
        flying = false;
        input_state.set_single_event('c', true);
    } else {
        flying = true;
        is_on_ground = false;
        crouching = false;
        height = NORMAL_HEIGHT;
        input_state.set_single_event('c', false);
    }
}

void Controller::toggle_crouch() {
    if (crouching) {
        crouching = false;
        height = NORMAL_HEIGHT;
    } else {
        sprinting = false;
        crouching = true;
        height = CROUCH_HEIGHT;
    }
}

void Controller::toggle_sprint() {
    if (sprinting) {
        sprinting = false;
    } else {
        crouching = false;
        sprinting = true;
        height = NORMAL_HEIGHT;
    }
}

bool Controller::is_block_solid(glm::vec3 sample_point) {
    return block_type::block_collidable[world_ptr->get_block(sample_point)->type];
}

#undef GRAVITY
#undef ACCELERATION
#undef AIR_ACCELERATION
#undef BRAKE_FACTOR
#undef AIR_BRAKE_FACTOR
#undef MAX_NORMAL_SPEED
#undef MAX_SPRINT_SPEED
#undef MAX_CROUCH_SPEED
#undef JUMP_STRENGTH
#undef TERMINAL_FALLING_VELOCITY
#undef NORMAL_HEIGHT
#undef CROUCH_HEIGHT
#undef VELOCITY_TERMINATOR

} /* end of namespace tc */
