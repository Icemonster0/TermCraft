#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "../glm.hpp"

#include "../render/mesh.hpp"

namespace tc {

namespace block_type {
    enum Block_Type {
        EMPTY,
        GRASS
    };

    const glm::vec3 block_color[] = {
        glm::vec3 {0.0f}, // EMPTY
        glm::vec3 {0.3f, 0.7f, 0.2f} // GRASS
    };
} /* end of namespace block_type */

struct block {
public:
    block() : type(block_type::EMPTY) {}

    block_type::Block_Type type;
    mesh block_mesh;
};

} /* end of namespace tc */

#endif /* end of include guard: BLOCK_HPP */
