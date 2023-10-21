#ifndef TRI_HPP
#define TRI_HPP

#include "../glm.hpp"

#include "vertex.hpp"

namespace tc {

struct tri {
    tri(vertex a, vertex b, vertex c) {
        vertices[0] = a;
        vertices[1] = b;
        vertices[2] = c;
        calc_normal();
    }

    tri() {
    }

    vertex vertices[3];
    glm::vec3 normal {0.0f};
    bool marked_for_death = false;

    void calc_normal() {
        normal = glm::cross(vertices[1].pos.xyz() - vertices[0].pos.xyz(),
                            vertices[2].pos.xyz() - vertices[0].pos.xyz());
    }
};

} /* end of namespace tc */

#endif /* end of include guard: TRI_HPP */
