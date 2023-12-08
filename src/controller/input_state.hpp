#ifndef INPUT_STATE_HPP
#define INPUT_STATE_HPP

#include "key_state.hpp"

#include <vector>
#include <algorithm>

#define KEY_TIMEOUT_DURATION 0.1f

namespace tc {

class Input_State {
public:
    Input_State() {
    }

    void time_step(float delta_time) {
        for (auto &key : key_states) {
            if (key.active) {
                key.time_left -= delta_time;
                if (key.time_left <= 0 || key.is_single_event) {
                    key.active = false;
                }
            }
        }
    }

    void activate_key(char c) {
        auto i = std::find_if(key_states.begin(), key_states.end(), [c](key_state k) {return k.key == c;});
        if (i != key_states.end()) {
            key_state &key = (*i);
            key.active = true;
            key.time_left = KEY_TIMEOUT_DURATION;
        }
    }

    bool get_key(char c) {
        auto i = std::find_if(key_states.begin(), key_states.end(), [c](key_state k) {return k.key == c;});
        if (i != key_states.end()) {
            return (*i).active;
        } else {
            return false;
        }
    }

    void add_key(char c) {
        key_states.emplace_back(c, false);
    }

    void add_single_event_key(char c) {
        key_states.emplace_back(c, true);
    }

    void set_single_event(char c, bool value) {
        auto i = std::find_if(key_states.begin(), key_states.end(), [c](key_state k) {return k.key == c;});
        if (i != key_states.end()) {
            (*i).is_single_event = value;
        }
    }

private:
    std::vector<key_state> key_states;
};

} /* end of namespace tc */

#undef KEY_TIMEOUT_DURATION

#endif /* end of include guard: INPUT_STATE_HPP */
