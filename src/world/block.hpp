#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "../glm.hpp"

#include "../render/mesh.hpp"
#include "../render/texture.hpp"

namespace tc {

namespace block_type {
    // Don't change the order of blocks in the lists!
    enum Block_Type {
        EMPTY,
        GRASS,
        DIRT,
        STONE,
    };
    const glm::vec3 block_color[] = {
        glm::vec3 {0.0f}, // EMPTY
        glm::vec3 {0.4f, 0.7f, 0.3f}, // GRASS
        glm::vec3 {0.5f, 0.4f, 0.2f}, // DIRT
        glm::vec3 {0.6f, 0.6f, 0.6f}, // STONE
    };
    const Texture_Set block_texture[] {
        {"../res/tex/test.png"}, // EMPTY
        {"../res/tex/grass_side.png", "../res/tex/grass_top.png", "../res/tex/dirt.png"}, // GRASS
        {"../res/tex/dirt.png"}, // DIRT
        {"../res/tex/stone.png"}, // STONE
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
