#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "../glm.hpp"

#include "block.hpp"
#include "../render/mesh.hpp"

#include <array>

namespace tc {

namespace chunk_size {
    const int width = 16;
    const int height = 256;
    const int depth = 16;
} /* end of namespace chunk_size */

class Chunk {
public:
    Chunk();

    std::array<std::array<std::array<block, chunk_size::depth>, chunk_size::height>, chunk_size::width> blocks;
    mesh chunk_mesh;
    bool loaded = true;
};

} /* end of namespace tc */

#endif /* end of include guard: CHUNK_HPP */
