#ifndef TRI_HPP
#define TRI_HPP

#include "../glm.hpp"

#include "vertex.hpp"

namespace tc {

struct tri {
    tri(const vertex &a, const  vertex &b, const  vertex &c) {
        vertices[0] = a;
        vertices[1] = b;
        vertices[2] = c;
    }

    tri() {
    }

    vertex vertices[3];
    bool marked_for_death = false;

    glm::vec3 world_normal {0.0f};
    glm::vec3 view_normal {0.0f};
    bool is_highlighted = false;
    unsigned int block_type_index = 0;

    glm::vec3 calc_normal() {
        return glm::cross(vertices[1].pos.xyz() - vertices[0].pos.xyz(),
                          vertices[2].pos.xyz() - vertices[0].pos.xyz());
    }
};

} /* end of namespace tc */

#endif /* end of include guard: TRI_HPP */
