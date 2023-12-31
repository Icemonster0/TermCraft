#include "world.hpp"

namespace tc {

// public:

void World::generate(int seed, glm::ivec2 size) {
    const int world_area = size.x * chunk_size::width * size.y * chunk_size::depth;

    int terrain_progress = 0;
    int terrain_percent = 0;
    printf("Generating World... (Initializing)\n");

    // initialize
    for (int i = 0; i < size.x; ++i) {
        chunks.emplace_back();
        for (int j = 0; j < size.y; ++j) {
            chunks[i].emplace_back();
        }
    }

    const int half_chunk_height = chunk_size::height / 2;

    #pragma omp parallel for schedule(static)
    for (int x = 0; x < size.x * chunk_size::width; ++x) {
        for (int y = 0; y < chunk_size::height; ++y) {
            for (int z = 0; z < size.y * chunk_size::depth; ++z) {
                block &b = *get_block({x, y, z});

                float mountainity = glm::clamp(glm::perlin(glm::vec3 {(float)x/120.0f, (float)z/120.0f, (float)seed*123.123f}) * 0.6f + 0.5f + glm::perlin(glm::vec3 {(float)x/3.0f, (float)z/3.0f, (float)seed*456.456f}) * 0.02f, 0.0f, 1.0f);

                int grass_height = (glm::perlin(glm::vec3 {(float)x/20.0f, (float)z/20.0f, (float)seed*689.689f}) * 0.5f + 0.5f) * mountainity * 30 + half_chunk_height;
                int dirt_height = grass_height - (mountainity > 0.45 ? 0 : 1);
                int stone_height = grass_height - int((0.7f - mountainity) * 5);

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
            terrain_progress++;
            terrain_percent = float(terrain_progress) / float(size.x * chunk_size::width - 1) * 100;
            printf("\rGenerating Terrain... %d%%", terrain_percent);
            fflush(stdout);
        }
    }
    printf("\n");

    std::mt19937 gen(seed);
    std::uniform_int_distribution x_dis(0, size.x * chunk_size::width);
    std::uniform_int_distribution y_dis(0, chunk_size::height);
    std::uniform_int_distribution z_dis(0, size.y * chunk_size::depth);
    std::uniform_real_distribution<float> f_dis(0.0f, 1.0f);
    std::uniform_real_distribution<float> norm_dis(-1.0f, 1.0f);

    // carve caves
    if (!U.no_caves) {
        std::mt19937 cave_gen(seed);
        const int max_caves = world_area / 1000;
        for (int i = 0; i < max_caves; ++i) {
            glm::vec3 current {x_dis(cave_gen), y_dis(cave_gen), z_dis(cave_gen)};
            glm::vec3 current_dir = glm::normalize(glm::vec3(norm_dis(cave_gen), norm_dis(cave_gen), norm_dis(cave_gen)));
            glm::vec3 next;
            glm::vec3 next_dir;

            // create curve (generate splines)
            const int cave_length = (f_dis(cave_gen) + 0.2f) * 20.0f;
            std::vector<Spline> splines;
            for (int j = 0; j < cave_length; ++j) {
                next = current + current_dir * (f_dis(cave_gen) + 0.3f) * 15.0f;
                next_dir = glm::normalize(current_dir + glm::vec3(norm_dis(cave_gen), norm_dis(cave_gen), norm_dis(cave_gen)));
                splines.emplace_back(current, current + current_dir * f_dis(cave_gen), next, next - next_dir * f_dis(cave_gen));
                current = next;
                current_dir = next_dir;
            }

            // carve out spheres along the curve
            const int subdivs = 10;
            const float radius = (f_dis(cave_gen) + 0.5f) * 5.0f;
            for (Spline spline : splines) {
                for (int j = 0; j < subdivs; ++j) {
                    float t = (float)j / (float)subdivs;
                    glm::vec3 p = spline.sample(t);

                    for (int x = -radius; x <= radius; ++x) {
                        for (int y = -radius; y <= radius; ++y) {
                            for (int z = -radius; z <= radius; ++z) {
                                if (x*x + y*y + z*z <= radius*radius) {
                                    get_block({p.x + x, p.y + y, p.z + z})->type = block_type::EMPTY;
                                }
                            }
                        }
                    }
                }
            }
            printf("\rGenerating Caves... %d%%", int((float)i / (float)max_caves * 100.0f));
        }
        printf("\n");
    }

    // scatter vegetation
    const int max_plants = world_area / 100;
    for (int i = 0; i < max_plants; ++i) {
        glm::ivec3 block;
        block.x = x_dis(gen);
        block.z = z_dis(gen);
        block.y = get_ground_height_at(block.xz());
        // Tuxes
        if (f_dis(gen) < 0.01f) {
            get_block(block + glm::ivec3(0, -1, 0))->type = block_type::TUX;
        }
        else if (get_block(block)->type == block_type::GRASS) {
            // Trees
            if (f_dis(gen) < 0.1f) {
                int tree_seed = glm::perlin(glm::vec3(block.x*2, block.z*2, (float)seed*77.0f+777.777f)) * 5000;
                place_tree(block, false, tree_seed);
                get_block(block)->type = block_type::DIRT;
            }
            // Flowers
            else {
                get_block(block + glm::ivec3(0, -1, 0))->type = block_type::FLOWER;
            }
        }
        printf("\rGenerating Plants... %d%%", int((float)i / (float)max_plants * 100.0f));
    }
    printf("\n");

    // calculate light levels
    int light_progress = 0;
    #pragma omp parallel for schedule(static)
    for (int x = 0; x < size.x * chunk_size::width; ++x) {
        for (int z = 0; z < size.y * chunk_size::depth; ++z) {
            update_sky_light_in_column({x, z});

            #pragma omp critical
            {
                light_progress++;
            }
        }
        #pragma omp critical
        {
            printf("\rCalculating Light Levels... %d%%", int(float(light_progress) / float(world_area) * 100.0f));
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
    get_block(highlighted_block)->is_highlighted = false;
    get_block(coord)->is_highlighted = true;

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

                        blocks_mesh_bytes += chunks[chunk_x][chunk_z].blocks[x][y][z].block_mesh->tri_list.capacity() * 3*sizeof(vertex);
                    }
                }
            }
        }
    }

    return blocks_bytes + world_mesh_bytes + chunks_mesh_bytes + blocks_mesh_bytes;
}

// private:

std::optional<glm::ivec2> World::get_chunk_coord_of_block(glm::ivec3 coord) {
    glm::ivec2 chunk_coord {floor(coord.x * chunk_size::inv_width), floor(coord.z * chunk_size::inv_depth)};

    if (chunk_coord.x < chunks.size() &&
        glm::all(glm::lessThan(chunk_coord, glm::ivec2(chunks.size(), chunks[chunk_coord.x].size()))) &&
        glm::all(glm::greaterThanEqual(chunk_coord, glm::ivec2(0, 0)))) {

        return std::optional<glm::ivec2> {chunk_coord};
    }
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

    update_sky_light_in_column(coord.xz());
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

void World::place_tree(glm::ivec3 coord, bool updates, int seed) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution dis(0, 3);

    const int height = 2 + dis(gen); // height of the trunk: random(2, 5)

    static std::vector<glm::ivec3> const leaves {
                    {-2,-1,-1}, {-2,-1, 0}, {-2,-1, 1},
        {-1,-1,-2}, {-1,-1,-1}, {-1,-1, 0}, {-1,-1, 1}, {-1,-1, 2},
        { 0,-1,-2}, { 0,-1,-1},             { 0,-1, 1}, { 0,-1, 2},
        { 1,-1,-2}, { 1,-1,-1}, { 1,-1, 0}, { 1,-1, 1}, { 1,-1, 2},
                    { 2,-1,-1}, { 2,-1, 0}, { 2,-1, 1},

                    {-2,-2,-1}, {-2,-2, 0}, {-2,-2, 1},
        {-1,-2,-2}, {-1,-2,-1}, {-1,-2, 0}, {-1,-2, 1}, {-1,-2, 2},
        { 0,-2,-2}, { 0,-2,-1},             { 0,-2, 1}, { 0,-2, 2},
        { 1,-2,-2}, { 1,-2,-1}, { 1,-2, 0}, { 1,-2, 1}, { 1,-2, 2},
                    { 2,-2,-1}, { 2,-2, 0}, { 2,-2, 1},

                                {-1,-3, 0},
                    { 0,-3,-1},             { 0,-3, 1},
                                { 1,-3, 0},

                                {-1,-4, 0},
                    { 0,-4,-1}, { 0,-4, 0}, { 0,-4, 1},
                                { 1,-4, 0},
    };

    static std::vector<glm::ivec3> const maybe_leaves {
        {-2,-1,-2},                                     {-2,-1, 2},
        { 2,-1,-2},                                     { 2,-1, 2},

        {-2,-2,-2},                                     {-2,-2, 2},
        { 2,-2,-2},                                     { 2,-2, 2},

                    {-1,-3,-1},             {-1,-3, 1},
                    { 1,-3,-1},             { 1,-3, 1},
    };

    // Trunk
    for (glm::ivec3 c {coord + glm::ivec3(0,-1,0)}; c.y >= coord.y-height-3; --c.y) {
        get_block(c)->type = block_type::OAK_LOG;
        if (updates) update_block(c);
    }

    // Guaranteed leaves
    for (glm::ivec3 c : leaves) {
        const glm::ivec3 block = coord + c + glm::ivec3(0, -height, 0);
        get_block(block)->type = block_type::OAK_LEAVES;
        if (updates) update_block(block);
    }

    // Random leaves
    for (glm::ivec3 c : maybe_leaves) {
        if (dis(gen) < 2) { // 50% chance
            const glm::ivec3 block = coord + c + glm::ivec3(0, -height, 0);
            get_block(block)->type = block_type::OAK_LEAVES;
            if (updates) update_block(block);
        }
    }
}

void World::update_sky_light_in_column(glm::ivec2 coord) {
    int light_level = 15;

    for (int y = 0; y < chunk_size::height; ++y) {
        block *b = get_block({coord.x, y, coord.y});

        b->sky_light = light_level;

        if (!block_type::block_transparent[b->type]) {
            bool decrease_light = true;
            for (int i = -1; i < 2; ++i) {
                for (int j = -1; j < 2; ++j) {
                    if (block_type::block_transparent[get_block({coord.x + i, y, coord.y + j})->type]) {
                        decrease_light = false;
                        i = 2;
                        j = 2;
                    }
                }
            }

            if (decrease_light) {
                light_level = max(light_level-1, 0);
            }
        }
    }
}

void World::remesh_block(glm::ivec3 coord) {
    block &b = *get_block(coord);
    b.block_mesh = std::make_unique<mesh>();

    if (b.type != block_type::EMPTY) {
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
        if (block_type::block_shape[b.type] == block_type::SOLID_BLOCK) {
            if (!neighbors[0][1][1]) { // left
                b.block_mesh->append(mesh_util::left_plane (neighbors, &b));
            }
            if (!neighbors[2][1][1]) { // right
                b.block_mesh->append(mesh_util::right_plane (neighbors, &b));
            }
            if (!neighbors[1][0][1]) { // top
                b.block_mesh->append(mesh_util::top_plane (neighbors, &b));
            }
            if (!neighbors[1][2][1]) { // bottom
                b.block_mesh->append(mesh_util::bottom_plane (neighbors, &b));
            }
            if (!neighbors[1][1][0]) { // front
                b.block_mesh->append(mesh_util::front_plane (neighbors, &b));
            }
            if (!neighbors[1][1][2]) { // back
                b.block_mesh->append(mesh_util::back_plane (neighbors, &b));
            }
        }
        else if (block_type::block_shape[b.type] == block_type::X_PLANES) {
            b.block_mesh->append(mesh_util::diagonal_plane (neighbors, &b, false));
            b.block_mesh->append(mesh_util::diagonal_plane (neighbors, &b, true));
        }
    }
}

void World::remesh_chunk(glm::ivec2 coord) {
    Chunk &chunk = chunks[coord.x][coord.y];
    chunk.chunk_mesh = mesh {};

    if (chunk.loaded) {

        int mesh_size = 0;
        for (int x = 0; x < chunk_size::width; ++x) {
            for (int y = 0; y < chunk_size::height; ++y) {
                for (int z = 0; z < chunk_size::depth; ++z) {
                    mesh_size += chunk.blocks[x][y][z].block_mesh->tri_list.size();
                }
            }
        }

        chunk.chunk_mesh.tri_list.reserve(mesh_size);

        // combining all block's meshes
        for (int x = 0; x < chunk_size::width; ++x) {
            for (int y = 0; y < chunk_size::height; ++y) {
                for (int z = 0; z < chunk_size::depth; ++z) {
                    chunk.chunk_mesh.append(
                        chunk.blocks[x][y][z].block_mesh->transform(glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)))
                    );
                }
            }
        }
    }
}

void World::remesh_world() {
    world_mesh = mesh {};

    int mesh_size = 0;
    for (int x = 0; x < chunks.size(); ++x) {
        for (int z = 0; z < chunks[x].size(); ++z) {
            mesh_size += chunks[x][z].chunk_mesh.tri_list.size();
        }
    }

    world_mesh.tri_list.reserve(mesh_size);

    for (int x = 0; x < chunks.size(); ++x) {
        for (int z = 0; z < chunks[x].size(); ++z) {
            world_mesh.append(
                chunks[x][z].chunk_mesh.transform(glm::translate(glm::mat4(1.0f), glm::vec3(x*chunk_size::width, 0, z*chunk_size::depth)))
            );
        }
    }
}

} /* end of namespace tc */
