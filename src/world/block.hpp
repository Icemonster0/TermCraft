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
        OAK_LOG,
        OAK_PLANKS,
    };
    const glm::vec3 block_color[] = {
        glm::vec3 {0.0f}, // EMPTY
        glm::vec3 {0.40f, 0.70f, 0.30f}, // GRASS
        glm::vec3 {0.50f, 0.40f, 0.20f}, // DIRT
        glm::vec3 {0.60f, 0.60f, 0.60f}, // STONE
        glm::vec3 {0.19f, 0.12f, 0.07f}, // OAK_LOG
        glm::vec3 {0.74f, 0.55f, 0.43f}, // OAK_PLANKS
    };
    const Texture_Set block_texture[] {
        {"../res/tex/test.png"}, // EMPTY
        {"../res/tex/grass_side.png", "../res/tex/grass_top.png", "../res/tex/dirt.png"}, // GRASS
        {"../res/tex/dirt.png"}, // DIRT
        {"../res/tex/stone.png"}, // STONE
        {"../res/tex/oak_log_side.png", "../res/tex/oak_log_top.png"}, // OAK_LOG
        {"../res/tex/oak_planks.png"}, // OAK_PLANKS
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
