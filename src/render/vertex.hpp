#ifndef VERTEX_HPP
#define VERTEX_HPP

#include "../glm.hpp"

namespace tc {

struct vertex {
    vertex(glm::vec3 p_pos) : pos(glm::vec4(p_pos, 1.0f)) {
    }

    vertex(glm::vec4 p_pos) : pos(p_pos) {
    }

    vertex(float x, float y, float z, bool ambient_occlusion = false, float s = 0.0f, float t = 0.0f)
        : pos(glm::vec4(x, y, z, 1.0f)), ao(ambient_occlusion), tex_coord(glm::vec2(s, t)) {
    }

    vertex(float x, float y, float z, float w) : pos(glm::vec4(x, y, z, w)) {
    }

    vertex() : pos(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) {
    }

    glm::vec4 pos;
    glm::vec2 screenpos {0.0f};
    glm::vec2 tex_coord {0.0f};
    float ao = 0.0f;
    float distance = 0.0f;
};

} /* end of namespace tc */

#endif /* end of include guard: VERTEX_HPP */
