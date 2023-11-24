#include "vertex.hpp"

namespace tc {

vertex::vertex(glm::vec3 p_pos) : pos(glm::vec4(p_pos, 1.0f)) {
}

vertex::vertex(glm::vec4 p_pos) : pos(p_pos) {
}

vertex::vertex(float x, float y, float z, bool ambient_occlusion, float s, float t)
    : pos(glm::vec4(x, y, z, 1.0f)), ao(ambient_occlusion), tex_coord(glm::vec2(s, t)) {
}

vertex::vertex(float x, float y, float z, float w) : pos(glm::vec4(x, y, z, w)) {
}

vertex::vertex() : pos(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) {
}

} /* end of namespace tc */
