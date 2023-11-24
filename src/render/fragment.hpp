#ifndef FRAGMENT_HPP
#define FRAGMENT_HPP

#include "../glm.hpp"
#include "tri.hpp"

namespace tc {

struct fragment {
    fragment(tri* t, float w0, float w1, float w2, float p_depth, float p_opacity = 1.0f);
    fragment();

    tri* triangle;
    float weights[3];
    float depth;
    float opacity;
};

} /* end of namespace tc */

#endif /* end of include guard: FRAGMENT_HPP */
