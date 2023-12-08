#include "fragment.hpp"

namespace tc {

fragment::fragment(tri* t, float w0, float w1, float w2, float p_depth, float p_opacity) : triangle(t), depth(p_depth), opacity(p_opacity) {
    weights[0] = w0;
    weights[1] = w1;
    weights[2] = w2;
}

fragment::fragment() {
}

} /* end of namespace tc */
