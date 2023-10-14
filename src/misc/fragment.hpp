#ifndef FRAGMENT_HPP
#define FRAGMENT_HPP

#include "../glm.hpp"

namespace tc {

struct fragment {
    fragment(glm::ivec2 p) : pos(p) {
    }

    fragment(int x, int y) : pos(glm::ivec2(x, y)) {
    }

    fragment() {
    }

    glm::ivec2 pos;
};

} /* end of namespace tc */

#endif /* end of include guard: FRAGMENT_HPP */
