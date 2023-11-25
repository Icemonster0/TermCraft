#ifndef TRI_HPP
#define TRI_HPP

#include "../glm.hpp"

#include "vertex.hpp"
#include "../world/block.hpp"

namespace tc {

struct block;

struct tri {
    tri(const vertex &a, const vertex &b, const vertex &c, block *ptr);
    tri();

    vertex vertices[3];
    bool marked_for_death = false;

    glm::vec3 world_normal {0.0f};
    glm::vec3 view_normal {0.0f};
    block *block_ptr;
    unsigned int block_side_index = 0;

    glm::vec3 calc_normal();
};

} /* end of namespace tc */

#endif /* end of include guard: TRI_HPP */
