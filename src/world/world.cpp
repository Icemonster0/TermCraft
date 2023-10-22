#include "world.hpp"
#include "block.hpp"

#include <cstdlib>
#include <cstdio>

namespace tc {

// public:

World::World(int seed) {
    generate(seed);
}

mesh World::get_mesh() {
    return world_mesh;
}

block* World::get_block(glm::ivec3 coord) {
    glm::ivec2 chunk_coord {coord.x / chunk_size::width, coord.z / chunk_size::depth};

    if (glm::all(glm::lessThan(chunk_coord, glm::ivec2(chunks.size(), chunks[chunk_coord.x].size()))) &&
        glm::all(glm::greaterThanEqual(chunk_coord, glm::ivec2(0, 0)))) {

        return &chunks[chunk_coord.x][chunk_coord.y].blocks[coord.x % chunk_size::width][coord.y][coord.z % chunk_size::depth];
    } else {
        null_block = block {};
        return &null_block;
    }
}

void World::replace(glm::ivec3 coord, block_type::Block_Type type) {
    get_block(coord)->type = type;
    update_block(coord);
}

int World::get_spawn_height() {
    int y;
    for (y = 0; y < chunk_size::height; ++y) {
        if (get_block({0, y, 0})->type != block_type::EMPTY)
            break;
    }
    return y;
}

// private:

void World::update_block(glm::ivec3 coord) {
    remesh_block(coord);

    remesh_block(coord + glm::ivec3(-1, 0, 0));
    remesh_block(coord + glm::ivec3( 1, 0, 0));
    remesh_block(coord + glm::ivec3(0, -1, 0));
    remesh_block(coord + glm::ivec3(0,  1, 0));
    remesh_block(coord + glm::ivec3(0, 0, -1));
    remesh_block(coord + glm::ivec3(0, 0,  1));

    glm::ivec2 chunk_coord {coord.x / chunk_size::width, coord.z / chunk_size::depth};
    glm::ivec3 relative_coord {coord.x % chunk_size::width, coord.y, coord.z % chunk_size::depth};

    remesh_chunk(chunk_coord);
    if (relative_coord.x == 0 && chunk_coord.x != 0)
        remesh_chunk(chunk_coord + glm::ivec2(-1, 0));
    else if (relative_coord.x == chunk_size::width-1 && chunk_coord.x != chunks.size()-1)
        remesh_chunk(chunk_coord + glm::ivec2( 1, 0));
    if (relative_coord.z == 0 && chunk_coord.y != 0)
        remesh_chunk(chunk_coord + glm::ivec2(0, -1));
    else if (relative_coord.z == chunk_size::depth-1 && chunk_coord.y != chunks[chunk_coord.x].size()-1)
        remesh_chunk(chunk_coord + glm::ivec2(0,  1));

    remesh_world();
}

void World::remesh_block(glm::ivec3 coord) {
    block &b = *get_block(coord);
    b.block_mesh = mesh {};

    if (b.type != block_type::EMPTY) {
        if (get_block(coord + glm::ivec3(-1, 0, 0))->type == block_type::EMPTY) { // left
            b.block_mesh.tri_list.emplace_back(tri {{0, 0, 0}, {0, 1, 1}, {0, 1, 0}});
            b.block_mesh.tri_list.emplace_back(tri {{0, 0, 0}, {0, 0, 1}, {0, 1, 1}});
        }
        if (get_block(coord + glm::ivec3(1, 0, 0))->type == block_type::EMPTY) { // right
            b.block_mesh.tri_list.emplace_back(tri {{1, 1, 1}, {1, 0, 1}, {1, 0, 0}});
            b.block_mesh.tri_list.emplace_back(tri {{1, 1, 1}, {1, 0, 0}, {1, 1, 0}});
        }
        if (get_block(coord + glm::ivec3(0, -1, 0))->type == block_type::EMPTY) { // top
            b.block_mesh.tri_list.emplace_back(tri {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}});
            b.block_mesh.tri_list.emplace_back(tri {{0, 0, 0}, {1, 0, 1}, {0, 0, 1}});
        }
        if (get_block(coord + glm::ivec3(0, 1, 0))->type == block_type::EMPTY) { // bottom
            b.block_mesh.tri_list.emplace_back(tri {{0, 1, 0}, {0, 1, 1}, {1, 1, 1}});
            b.block_mesh.tri_list.emplace_back(tri {{0, 1, 0}, {1, 1, 1}, {1, 1, 0}});
        }
        if (get_block(coord + glm::ivec3(0, 0, -1))->type == block_type::EMPTY) { // front
            b.block_mesh.tri_list.emplace_back(tri {{0, 1, 0}, {1, 1, 0}, {1, 0, 0}});
            b.block_mesh.tri_list.emplace_back(tri {{0, 1, 0}, {1, 0, 0}, {0, 0, 0}});
        }
        if (get_block(coord + glm::ivec3(0, 0, 1))->type == block_type::EMPTY) { // back
            b.block_mesh.tri_list.emplace_back(tri {{1, 0, 1}, {1, 1, 1}, {0, 1, 1}});
            b.block_mesh.tri_list.emplace_back(tri {{1, 0, 1}, {0, 1, 1}, {0, 0, 1}});
        }
    }
}

void World::remesh_chunk(glm::ivec2 coord) {
    Chunk &chunk = chunks[coord.x][coord.y];
    chunk.chunk_mesh = mesh {};

    for (int x = 0; x < chunk_size::width; ++x) {
        for (int y = 0; y < chunk_size::height; ++y) {
            for (int z = 0; z < chunk_size::depth; ++z) {
                chunk.chunk_mesh.append(
                    chunk.blocks[x][y][z].block_mesh.transform(glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)))
                );
            }
        }
    }
}

void World::remesh_world() {
    world_mesh = mesh {};

    for (int x = 0; x < chunks.size(); ++x) {
        for (int z = 0; z < chunks[x].size(); ++z) {
    // for (auto &row : chunks) {
    //     for (Chunk &chunk : row) {
            world_mesh.append(
                chunks[x][z].chunk_mesh.transform(glm::translate(glm::mat4(1.0f), glm::vec3(x*chunk_size::width, 0, z*chunk_size::depth)))
            );
        }
    }
}

void World::generate(int seed) {
    srand(seed);

    for (int i = 0; i < 3; ++i) {
        chunks.emplace_back();
        for (int j = 0; j < 3; ++j) {
            chunks[i].emplace_back();
        }
    }

    for (int chunk_x = 0; chunk_x < chunks.size(); ++chunk_x) {
        for (int chunk_z = 0; chunk_z < chunks[chunk_x].size(); ++chunk_z) {
            Chunk &chunk = chunks[chunk_x][chunk_z];

            for (int x = 0; x < chunk_size::width; ++x) {
                for (int y = 0; y < chunk_size::height; ++y) {
                    for (int z = 0; z < chunk_size::depth; ++z) {
                        block &b = chunk.blocks[x][y][z];

                        if (y >= 128) {
                            b.type = block_type::GRASS;
                        } else {
                            b.type = block_type::EMPTY;
                        }
                    }
                }
            }
        }
    }

    generate_initial_mesh();
}

void World::generate_initial_mesh() {
    for (int x = 0; x < chunks.size() * chunk_size::width; ++x) {
        for (int y = 0; y < chunk_size::height; ++y) {
            for (int z = 0; z < chunks[x / chunk_size::width].size() * chunk_size::depth; ++z) {
                remesh_block({x, y, z});
            }
        }
    }

    for (int chunk_x = 0; chunk_x < chunks.size(); ++chunk_x) {
        for (int chunk_z = 0; chunk_z < chunks[chunk_x].size(); ++chunk_z) {
            remesh_chunk({chunk_x, chunk_z});
        }
    }

    remesh_world();
}

} /* end of namespace tc */