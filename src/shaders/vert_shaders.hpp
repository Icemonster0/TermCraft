#ifndef VERT_SHADERS_HPP
#define VERT_SHADERS_HPP

#include "../glm.hpp"

#include "../misc/vertex.hpp"

#include <cmath>

namespace tc {

struct vert_shaders {
    static vertex VERT_default(vertex v, float global_time) {
        return v;
    }

    static vertex VERT_fun(vertex v, float global_time) {
        return vertex {v.pos * (sin(global_time*3)*0.5+0.5) * 2};
    }
};

} /* end of namespace tc */

#endif /* end of include guard: VERT_SHADERS_HPP */
