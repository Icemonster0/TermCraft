#include "../glm.hpp"

#include "chunk.hpp"

namespace tc {

Chunk::Chunk() {
}

block* Chunk::get_block(glm::ivec3 coord) {
    if (glm::all(glm::lessThan(coord, glm::ivec3(chunk_size::width, chunk_size::height, chunk_size::depth))) &&
        glm::all(glm::greaterThanEqual(coord, glm::ivec3(0, 0, 0)))) {

        return &blocks[coord.x][coord.y][coord.z];
    } else {
        return nullptr;
    }
}


} /* end of namespace tc */
