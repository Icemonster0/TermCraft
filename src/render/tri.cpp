#include "tri.hpp"

namespace tc {

tri::tri(const vertex &a, const vertex &b, const vertex &c, block *ptr) : block_ptr(ptr) {
    vertices[0] = a;
    vertices[1] = b;
    vertices[2] = c;
}

tri::tri() {
}

glm::vec3 tri::calc_normal() {
    return glm::cross(vertices[1].pos.xyz() - vertices[0].pos.xyz(),
                      vertices[2].pos.xyz() - vertices[0].pos.xyz());
}

} /* end of namespace tc */
