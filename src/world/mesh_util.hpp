#ifndef MESH_UTIL_HPP
#define MESH_UTIL_HPP

#include "../glm.hpp"

#include "../render/mesh.hpp"
#include "block.hpp"

namespace tc::mesh_util {

mesh left_plane(glm::bvec4 ao, block_type::Block_Type type) {
    mesh m;
    m.tri_list.emplace_back(tri {{0, 0, 0, ao.x, 1, 0}, {0, 0, 1, ao.y, 0, 0}, {0, 1, 1, ao.z, 0, 1}});
    m.tri_list.emplace_back(tri {{0, 0, 0, ao.x, 1, 0}, {0, 1, 1, ao.z, 0, 1}, {0, 1, 0, ao.w, 1, 1}});
    m.tri_list[0].vertices[0].color = m.tri_list[0].vertices[1].color = m.tri_list[0].vertices[2].color
     = m.tri_list[1].vertices[0].color = m.tri_list[1].vertices[1].color = m.tri_list[1].vertices[2].color
     = block_type::block_color[type];
    m.tri_list[0].block_type_index = type;
    m.tri_list[1].block_type_index = type;
    return m;
}
mesh right_plane(glm::bvec4 ao, block_type::Block_Type type) {
    mesh m;
    m.tri_list.emplace_back(tri {{1, 1, 1, ao.x, 1, 1}, {1, 0, 1, ao.y, 1, 0}, {1, 0, 0, ao.z, 0, 0}});
    m.tri_list.emplace_back(tri {{1, 1, 1, ao.x, 1, 1}, {1, 0, 0, ao.z, 0, 0}, {1, 1, 0, ao.w, 0, 1}});
    m.tri_list[0].vertices[0].color = m.tri_list[0].vertices[1].color = m.tri_list[0].vertices[2].color
     = m.tri_list[1].vertices[0].color = m.tri_list[1].vertices[1].color = m.tri_list[1].vertices[2].color
     = block_type::block_color[type];
    m.tri_list[0].block_type_index = type;
    m.tri_list[1].block_type_index = type;
    return m;
}
mesh top_plane(glm::bvec4 ao, block_type::Block_Type type) {
    mesh m;
    m.tri_list.emplace_back(tri {{0, 0, 0, ao.x, 0, 1}, {1, 0, 0, ao.y, 1, 1}, {1, 0, 1, ao.z, 1, 0}});
    m.tri_list.emplace_back(tri {{0, 0, 0, ao.x, 0, 1}, {1, 0, 1, ao.z, 1, 0}, {0, 0, 1, ao.w, 0, 0}});
    m.tri_list[0].vertices[0].color = m.tri_list[0].vertices[1].color = m.tri_list[0].vertices[2].color
     = m.tri_list[1].vertices[0].color = m.tri_list[1].vertices[1].color = m.tri_list[1].vertices[2].color
     = block_type::block_color[type];
    m.tri_list[0].block_type_index = type;
    m.tri_list[1].block_type_index = type;
    return m;
}
mesh bottom_plane(glm::bvec4 ao, block_type::Block_Type type) {
    mesh m;
    m.tri_list.emplace_back(tri {{0, 1, 0, ao.x, 0, 0}, {0, 1, 1, ao.y, 0, 1}, {1, 1, 1, ao.z, 1, 1}});
    m.tri_list.emplace_back(tri {{0, 1, 0, ao.x, 0, 0}, {1, 1, 1, ao.z, 1, 1}, {1, 1, 0, ao.w, 1, 0}});
    m.tri_list[0].vertices[0].color = m.tri_list[0].vertices[1].color = m.tri_list[0].vertices[2].color
     = m.tri_list[1].vertices[0].color = m.tri_list[1].vertices[1].color = m.tri_list[1].vertices[2].color
     = block_type::block_color[type];
    m.tri_list[0].block_type_index = type;
    m.tri_list[1].block_type_index = type;
    return m;
}
mesh front_plane(glm::bvec4 ao, block_type::Block_Type type) {
    mesh m;
    m.tri_list.emplace_back(tri {{0, 1, 0, ao.x, 0, 1}, {1, 1, 0, ao.y, 1, 1}, {1, 0, 0, ao.z, 1, 0}});
    m.tri_list.emplace_back(tri {{0, 1, 0, ao.x, 0, 1}, {1, 0, 0, ao.z, 1, 0}, {0, 0, 0, ao.w, 0, 0}});
    m.tri_list[0].vertices[0].color = m.tri_list[0].vertices[1].color = m.tri_list[0].vertices[2].color
     = m.tri_list[1].vertices[0].color = m.tri_list[1].vertices[1].color = m.tri_list[1].vertices[2].color
     = block_type::block_color[type];
    m.tri_list[0].block_type_index = type;
    m.tri_list[1].block_type_index = type;
    return m;
}
mesh back_plane(glm::bvec4 ao, block_type::Block_Type type) {
    mesh m;
    m.tri_list.emplace_back(tri {{1, 0, 1, ao.x, 0, 0}, {1, 1, 1, ao.y, 0, 1}, {0, 1, 1, ao.z, 1, 1}});
    m.tri_list.emplace_back(tri {{1, 0, 1, ao.x, 0, 0}, {0, 1, 1, ao.z, 1, 1}, {0, 0, 1, ao.w, 1, 0}});
    m.tri_list[0].vertices[0].color = m.tri_list[0].vertices[1].color = m.tri_list[0].vertices[2].color
     = m.tri_list[1].vertices[0].color = m.tri_list[1].vertices[1].color = m.tri_list[1].vertices[2].color
     = block_type::block_color[type];
    m.tri_list[0].block_type_index = type;
    m.tri_list[1].block_type_index = type;
    return m;
}

} /* end of namespace tc::mesh_util */

#endif /* end of include guard: MESH_UTIL_HPP */
