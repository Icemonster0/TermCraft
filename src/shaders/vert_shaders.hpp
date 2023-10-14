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
        glm::mat4 transform(1.0f);
        transform = glm::rotate(transform, glm::radians(10.0f*global_time), glm::vec3(0, 0, 1));
        // transform = glm::scale(transform, glm::vec3((sin(global_time*3)*0.5+0.5) * 2));
        return vertex {transform * v.pos};
    }
};

} /* end of namespace tc */

#endif /* end of include guard: VERT_SHADERS_HPP */
