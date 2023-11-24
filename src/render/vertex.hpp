#ifndef VERTEX_HPP
#define VERTEX_HPP

#include "../glm.hpp"

namespace tc {

struct vertex {
    vertex(glm::vec3 p_pos);
    vertex(glm::vec4 p_pos);
    vertex(float x, float y, float z, bool ambient_occlusion = false, float s = 0.0f, float t = 0.0f);
    vertex(float x, float y, float z, float w);
    vertex();

    glm::vec4 pos;
    glm::vec2 screenpos {0.0f};
    glm::vec2 tex_coord {0.0f};
    float ao = 0.0f;
    float distance = 0.0f;
};

} /* end of namespace tc */

#endif /* end of include guard: VERTEX_HPP */
