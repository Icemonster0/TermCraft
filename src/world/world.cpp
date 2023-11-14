#include "world.hpp"
#include "block.hpp"
#include "mesh_util.hpp"

#include <cstdlib>
#include <cstdio>
#include <optional>

namespace tc {

// public:

void World::generate(int seed, glm::ivec2 size) {
    int progress = 0;
    int percent = 0;
    printf("Generating World... (Initializing)\n");

    srand(seed);

    for (int i = 0; i < size.x; ++i) {
        chunks.emplace_back();
        for (int j = 0; j < size.y; ++j) {
            chunks[i].emplace_back();
        }
    }

    const int half_chunk_height = chunk_size::height / 2;

    #pragma omp parallel for schedule(static)
    for (int x = 0; x < chunks.size() * chunk_size::width; ++x) {
        for (int y = 0; y < chunk_size::height; ++y) {
            for (int z = 0; z < chunks[x / chunk_size::width].size() * chunk_size::depth; ++z) {
                block &b = *get_block({x, y, z});

                float mountainity = glm::perlin(glm::vec2 {(float)x/120.0f, (float)z/120.0f}) * 0.5f + 0.5f;

                int grass_height = (glm::perlin(glm::vec2 {(float)x/20.0f, (float)z/20.0f}) * 0.5f + 0.5f) * mountainity * 30 + half_chunk_height;
                int dirt_height = grass_height - 1;
                int stone_height = dirt_height - 5;

                if (y >= chunk_size::height - stone_height) {
                    b.type = block_type::STONE;
                } else if (y >= chunk_size::height - dirt_height) {
                    b.type = block_type::DIRT;
                } else if (y >= chunk_size::height - grass_height) {
                    b.type = block_type::GRASS;
                } else {
                    b.type = block_type::EMPTY;
                }
            }
        }

        #pragma omp critical
        {
            progress++;
            percent = float(progress) / float(size.x * chunk_size::width - 1) * 100;
            printf("\rGenerating World... %d%%", percent);
            fflush(stdout);
        }
    }
    printf("\n");
}

void World::generate_initial_mesh() {
    int progress = 0;
    int percent = 0;
    printf("Generating Mesh... %d%%", percent);
    fflush(stdout);

    #pragma omp parallel for schedule(static)
    for (int x = 0; x < chunks.size() * chunk_size::width; ++x) {
        for (int y = 0; y < chunk_size::height; ++y) {
            for (int z = 0; z < chunks[x / chunk_size::width].size() * chunk_size::depth; ++z) {
                remesh_block({x, y, z});
            }
        }

        #pragma omp critical
        {
            progress++;
            percent = float(progress) / float(chunks.size() * chunk_size::width - 1) * 100;
            printf("\rGenerating Mesh... %d%%", percent);
            fflush(stdout);
        }
    }
    printf("\n");

    for (int chunk_x = 0; chunk_x < chunks.size(); ++chunk_x) {
        for (int chunk_z = 0; chunk_z < chunks[chunk_x].size(); ++chunk_z) {
            remesh_chunk({chunk_x, chunk_z});
        }
    }

    remesh_world();
}

mesh World::get_mesh() {
    return world_mesh;
}

block* World::get_block(glm::ivec3 coord) {
    std::optional<glm::ivec2> chunk_coord = get_chunk_coord_of_block(coord);

    if (chunk_coord.has_value()) {
        glm::ivec3 relative_coord {
            coord.x - chunk_size::width*chunk_coord.value().x,
            coord.y,
            coord.z - chunk_size::depth*chunk_coord.value().y
        };

        if (glm::all(glm::greaterThanEqual(relative_coord, glm::ivec3(0))) &&
            glm::all(glm::lessThan(relative_coord, glm::ivec3(chunk_size::width, chunk_size::height, chunk_size::depth)))) {

            return &chunks[chunk_coord.value().x][chunk_coord.value().y]
                    .blocks[relative_coord.x][relative_coord.y][relative_coord.z];
        }
    }

    null_block = block {};
    return &null_block;
}

void World::replace(glm::ivec3 coord, block_type::Block_Type type) {
    get_block(coord)->type = type;
    update_block(coord);
}

void World::highlight_block(glm::ivec3 coord) {
    block &old_block = *get_block(highlighted_block);
    for (tri &triangle : old_block.block_mesh.tri_list) {
        triangle.is_highlighted = false;
    }
    block &new_block = *get_block(coord);
    for (tri &triangle : new_block.block_mesh.tri_list) {
        triangle.is_highlighted = true;
    }

    std::optional<glm::ivec2> coord_chunk = get_chunk_coord_of_block(coord);
    std::optional<glm::ivec2> highlighted_block_chunk = get_chunk_coord_of_block(highlighted_block);

    if (coord_chunk.has_value() && highlighted_block_chunk.has_value() &&
        glm::all(glm::equal(coord_chunk.value(), highlighted_block_chunk.value()))) {

        remesh_chunk(coord_chunk.value());
    } else {
        if (coord_chunk.has_value()) remesh_chunk(coord_chunk.value());
        if (highlighted_block_chunk.has_value()) remesh_chunk(highlighted_block_chunk.value());
    }

    remesh_world();

    highlighted_block = coord;
}

int World::get_ground_height_at(glm::ivec2 coord) {
    int y;
    for (y = 0; y < chunk_size::height; ++y) {
        if (get_block({coord.x, y, coord.y})->type != block_type::EMPTY)
            break;
    }
    return y;
}

glm::ivec2 World::get_world_center() {
    return {chunks.size() * chunk_size::width / 2,
            chunks[chunks.size() / 2].size() * chunk_size::depth / 2};
}

void World::update_chunks(glm::vec3 new_player_pos, glm::vec3 old_player_pos, float render_dist) {
    /* If the player moves from one chunk to another,
     * we remesh the loaded chunk difference (boolean operation). */

    glm::ivec2 new_chunk_coord = get_chunk_coord_of_block(new_player_pos).value_or(glm::ivec2(-1));
    glm::ivec2 old_chunk_coord = get_chunk_coord_of_block(old_player_pos).value_or(glm::ivec2(-1));

    if (!glm::all(glm::equal(new_chunk_coord, old_chunk_coord))) {

        // distance culling
        for (glm::ivec2 chunk_coord {0}; chunk_coord.x < chunks.size(); ++chunk_coord.x) {
            for (chunk_coord.y = 0; chunk_coord.y < chunks[chunk_coord.x].size(); ++chunk_coord.y) {

                glm::vec2 diff = {((chunk_coord.x+0.5f) * chunk_size::width) - new_player_pos.x,
                                  ((chunk_coord.y+0.5f) * chunk_size::depth) - new_player_pos.z};

                bool was_loaded = chunks[chunk_coord.x][chunk_coord.y].loaded;

                if (diff.x*diff.x + diff.y*diff.y <= render_dist*render_dist) {
                    chunks[chunk_coord.x][chunk_coord.y].loaded = true;
                    if (!was_loaded)
                        remesh_chunk(chunk_coord);
                }
                else {
                    chunks[chunk_coord.x][chunk_coord.y].loaded = false;
                    if (was_loaded)
                        remesh_chunk(chunk_coord);
                }
            }
        }

        remesh_world();
    }
}

size_t World::estimate_memory_usage() {
    size_t blocks_bytes = sizeof(block) * chunk_size::width * chunk_size::height * chunk_size::depth * chunks.size() * chunks[0].size();
    size_t world_mesh_bytes = world_mesh.tri_list.capacity() * 3*sizeof(vertex);

    size_t chunks_mesh_bytes = 0;
    size_t blocks_mesh_bytes = 0;

    for (int chunk_x = 0; chunk_x < chunks.size(); ++chunk_x) {
        for (int chunk_z = 0; chunk_z < chunks[chunk_x].size(); ++chunk_z) {

            chunks_mesh_bytes += chunks[chunk_x][chunk_z].chunk_mesh.tri_list.capacity() * 3*sizeof(vertex);

            for (int x = 0; x < chunk_size::width; ++x) {
                for (int y = 0; y < chunk_size::height; ++y) {
                    for (int z = 0; z < chunk_size::depth; ++z) {

                        blocks_mesh_bytes += chunks[chunk_x][chunk_z].blocks[x][y][z].block_mesh.tri_list.capacity() * 3*sizeof(vertex);
                    }
                }
            }
        }
    }

    return blocks_bytes + world_mesh_bytes + chunks_mesh_bytes + blocks_mesh_bytes;
}

// private:

std::optional<glm::ivec2> World::get_chunk_coord_of_block(glm::ivec3 coord) {
    glm::ivec2 chunk_coord {coord.x / chunk_size::width, coord.z / chunk_size::depth};

    if (glm::all(glm::lessThan(chunk_coord, glm::ivec2(chunks.size(), chunks[chunk_coord.x].size()))) &&
        glm::all(glm::greaterThanEqual(chunk_coord, glm::ivec2(0, 0))))
         return std::optional<glm::ivec2> {chunk_coord};
    else return std::optional<glm::ivec2> {};
}

void World::update_block(glm::ivec3 coord) {
    block_update_simulation(coord);

    // use this if ao doesn't matter
    // remesh_block(coord);
    // remesh_block(coord + glm::ivec3(-1, 0, 0));
    // remesh_block(coord + glm::ivec3( 1, 0, 0));
    // remesh_block(coord + glm::ivec3(0, -1, 0));
    // remesh_block(coord + glm::ivec3(0,  1, 0));
    // remesh_block(coord + glm::ivec3(0, 0, -1));
    // remesh_block(coord + glm::ivec3(0, 0,  1));

    // use this if ao matters (which it does)
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            for (int z = -1; z <= 1; ++z) {
                remesh_block(coord + glm::ivec3(x, y, z));
            }
        }
    }

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

void World::block_update_simulation(glm::ivec3 coord) {
    block *block_current = get_block(coord);
    block *block_below = get_block(coord + glm::ivec3(0, 1, 0));

    // Replace grass with dirt
    if (block_current->type == block_type::GRASS &&
        !block_type::block_transparent[get_block(coord + glm::ivec3(0, -1, 0))->type]) {

        block_current->type = block_type::DIRT;
    }
    if (block_below->type == block_type::GRASS &&
        !block_type::block_transparent[block_current->type]) {
        block_below->type = block_type::DIRT;
    }
}

void World::remesh_block(glm::ivec3 coord) {
    block &b = *get_block(coord);
    b.block_mesh = mesh {};

    // store in array which neighbors exist
    bool neighbors[3][3][3];
    for (int x = 0; x < 3; ++x) {
        for (int y = 0; y < 3; ++y) {
            for (int z = 0; z < 3; ++z) {
                neighbors[x][y][z] = !block_type::block_transparent[get_block(coord + glm::ivec3(x-1, y-1, z-1))->type];
            }
        }
    }

    // generate faces
    if (b.type != block_type::EMPTY) {
        if (!neighbors[0][1][1]) { // left
            b.block_mesh.append(mesh_util::left_plane (neighbors, b.type));
        }
        if (!neighbors[2][1][1]) { // right
            b.block_mesh.append(mesh_util::right_plane (neighbors, b.type));
        }
        if (!neighbors[1][0][1]) { // top
            b.block_mesh.append(mesh_util::top_plane (neighbors, b.type));
        }
        if (!neighbors[1][2][1]) { // bottom
            b.block_mesh.append(mesh_util::bottom_plane (neighbors, b.type));
        }
        if (!neighbors[1][1][0]) { // front
            b.block_mesh.append(mesh_util::front_plane (neighbors, b.type));
        }
        if (!neighbors[1][1][2]) { // back
            b.block_mesh.append(mesh_util::back_plane (neighbors, b.type));
        }
    }
}

void World::remesh_chunk(glm::ivec2 coord) {
    Chunk &chunk = chunks[coord.x][coord.y];
    chunk.chunk_mesh = mesh {};

    if (chunk.loaded) {
        // combining all block's meshes
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
}

void World::remesh_world() {
    world_mesh = mesh {};

    for (int x = 0; x < chunks.size(); ++x) {
        for (int z = 0; z < chunks[x].size(); ++z) {
            world_mesh.append(
                chunks[x][z].chunk_mesh.transform(glm::translate(glm::mat4(1.0f), glm::vec3(x*chunk_size::width, 0, z*chunk_size::depth)))
            );
        }
    }
}

} /* end of namespace tc */
