#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "../glm.hpp"

#include "../render/mesh.hpp"
#include "../render/texture.hpp"

#include <memory>

namespace tc {

namespace block_type {
    enum Block_Shape {
        SOLID_BLOCK,
        X_PLANES,
    };

    // Don't change the order of blocks in the lists!
    enum Block_Type {
        EMPTY,
        GRASS,
        DIRT,
        STONE,
        OAK_LOG,
        OAK_PLANKS,
        FLOWER,
        TUX,
    };
    const glm::vec3 block_color[] = {
        glm::vec3 {0.0f}, // EMPTY
        glm::vec3 {0.40f, 0.70f, 0.30f}, // GRASS
        glm::vec3 {0.50f, 0.40f, 0.20f}, // DIRT
        glm::vec3 {0.60f, 0.60f, 0.60f}, // STONE
        glm::vec3 {0.19f, 0.12f, 0.07f}, // OAK_LOG
        glm::vec3 {0.74f, 0.55f, 0.43f}, // OAK_PLANKS
        glm::vec3 {1.00f, 0.10f, 0.10f}, // FLOWER
        glm::vec3 {1.00f, 1.00f, 1.00f}, // TUX
    };
    const Texture_Set block_texture[] {
        {"res/tex/test.png"}, // EMPTY
        {"res/tex/grass_side.png", "res/tex/grass_top.png", "res/tex/dirt.png"}, // GRASS
        {"res/tex/dirt.png"}, // DIRT
        {"res/tex/stone.png"}, // STONE
        {"res/tex/oak_log_side.png", "res/tex/oak_log_top.png"}, // OAK_LOG
        {"res/tex/oak_planks.png"}, // OAK_PLANKS
        {"res/tex/flower.png"}, // FLOWER
        {"res/tex/Tux.png"}, // TUX
    };
    const bool block_transparent[] {
        true, // EMPTY
        false, // GRASS
        false, // DIRT
        false, // STONE
        false, // OAK_LOG
        false, // OAK_PLANKS
        true, // FLOWER
        true, // TUX
    };
    const bool block_shape[] {
        SOLID_BLOCK, // EMPTY
        SOLID_BLOCK, // GRASS
        SOLID_BLOCK, // DIRT
        SOLID_BLOCK, // STONE
        SOLID_BLOCK, // OAK_LOG
        SOLID_BLOCK, // OAK_PLANKS
        X_PLANES, // FLOWER
        X_PLANES, // TUX
    };
    const char block_initial[] {
        ' ', // EMPTY
        'G', // GRASS
        'D', // DIRT
        'S', // STONE
        'L', // OAK_LOG
        'P', // OAK_PLANKS
        'F', // FLOWER
        'T', // TUX
    };
} /* end of namespace block_type */

struct mesh;

struct block {
public:
    block();

    block_type::Block_Type type;
    bool is_highlighted;
    std::unique_ptr<mesh> block_mesh;
};

} /* end of namespace tc */

#endif /* end of include guard: BLOCK_HPP */
