#ifndef WORLD_HPP
#define WORLD_HPP

#include "../glm.hpp"

#include "chunk.hpp"
#include "block.hpp"
#include "../render/mesh.hpp"

#include <vector>

namespace tc {

class World {
public:
    World(int seed);
    World() {}

    mesh get_mesh();
    block* get_block(glm::ivec3 coord);
    void replace(glm::ivec3 coord, block_type::Block_Type type);
    void highlight_block(glm::ivec3 coord);
    int get_ground_height_at(glm::ivec2 coord);
    glm::ivec2 get_world_center();

private:
    glm::ivec2 get_chunk_of_block(glm::ivec3 coord);
    void update_block(glm::ivec3 coord);
    void remesh_block(glm::ivec3 coord);
    void remesh_chunk(glm::ivec2 coord);
    void remesh_world();
    void generate(int seed);
    void generate_initial_mesh();

    std::vector<std::vector<Chunk>> chunks;
    mesh world_mesh;
    block null_block; // is returned for invalid coords
    glm::ivec3 highlighted_block {-1};
};

} /* end of namespace tc */

#endif /* end of include guard: WORLD_HPP */
