#ifndef VERT_SHADERS_HPP
#define VERT_SHADERS_HPP

#include "../glm.hpp"

#include "../render/vertex.hpp"

#include <cmath>

namespace tc {

struct vert_shaders {
    static void VERT_default(vertex *v, float global_time) {
    }

    static void VERT_fun(vertex *v, float global_time) {
        glm::mat4 transform(1.0f);
        transform = glm::rotate(transform, glm::radians(90.0f*global_time), glm::vec3(0, 0, 1));
        transform = glm::scale(transform, glm::vec3((sin(global_time)) * 2));
        v->pos = transform * v->pos;
    }
};

} /* end of namespace tc */

#endif /* end of include guard: VERT_SHADERS_HPP */
