#ifndef KEY_STATE_HPP
#define KEY_STATE_HPP

namespace tc {

struct key_state {
    key_state(char p_key, bool p_is_single_event)
      : key(p_key), is_single_event(p_is_single_event),
        time_left(KEY_TIMEOUT_DURATION), active(false) {
    }

    key_state() {}

    char key;
    bool is_single_event;
    float time_left;
    bool active;
};

} /* end of namespace tc */

#endif /* end of include guard: KEY_STATE_HPP */
