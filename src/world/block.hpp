#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "../render/mesh.hpp"
#include "../render/tri.hpp"
#include "../render/vertex.hpp"

namespace tc {

namespace block_type {
    enum Block_Type {
        EMPTY,
        GRASS
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
