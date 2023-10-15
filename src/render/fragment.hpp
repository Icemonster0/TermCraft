#ifndef FRAGMENT_HPP
#define FRAGMENT_HPP

#include "../glm.hpp"
#include "tri.hpp"

namespace tc {

struct fragment {
    fragment(tri* t, float w0, float w1, float w2) : triangle(t) {
        weights[0] = w0;
        weights[1] = w1;
        weights[2] = w2;
    }

    fragment() {
    }

    tri* triangle;
    float weights[3];
};

} /* end of namespace tc */

#endif /* end of include guard: FRAGMENT_HPP */
