#ifndef INPUT_STATE_HPP
#define INPUT_STATE_HPP

#define KEY_TIMEOUT_DURATION 0.1f
#include "key_state.hpp"

#include <vector>

namespace tc {

class Input_State {
public:
    Input_State() {
    }

    void time_step(float delta_time) {
        for (auto &key : key_states) {
            if (key.active) {
                key.time_left -= delta_time;
                if (key.time_left <= 0) {
                    key.active = false;
                }
            }
        }
    }

    void activate_key(char c) {
        for (auto &key : key_states) {
            if (key.key == c) {
                key.active = true;
                key.time_left = KEY_TIMEOUT_DURATION;
                return;
            }
        }
    }

    bool get_key(char c) {
        for (auto &key : key_states) {
            if (key.key == c) {
                return key.active;
            }
        }
        return false;
    }

    void add_key(char c) {
        key_states.emplace_back(c);
    }

private:
    std::vector<key_state> key_states;
};

} /* end of namespace tc */

#endif /* end of include guard: INPUT_STATE_HPP */
