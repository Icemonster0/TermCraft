#ifndef KEY_STATE_HPP
#define KEY_STATE_HPP

namespace tc {

struct key_state {
    key_state(char p_key) : key(p_key), time_left(KEY_TIMEOUT_DURATION), active(false) {
    }

    key_state() {}

    char key;
    float time_left;
    bool active;
};

} /* end of namespace tc */

#endif /* end of include guard: KEY_STATE_HPP */
