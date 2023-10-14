#ifndef VERT_SHADERS_HPP
#define VERT_SHADERS_HPP

#include "../glm.hpp"

#include "../misc/vertex.hpp"

namespace tc {

struct vert_shaders {
    static vertex default(vertex v) {
        return v;
    }
};

} /* end of namespace tc */

#endif /* end of include guard: VERT_SHADERS_HPP */
