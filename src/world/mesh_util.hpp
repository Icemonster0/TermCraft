#ifndef MESH_UTIL_HPP
#define MESH_UTIL_HPP

#include "../glm.hpp"

#include "../render/mesh.hpp"
#include "block.hpp"

#include <cstdio>
#include <unistd.h>

namespace tc::mesh_util {

glm::bvec4 calc_ambient_occlusion(bool nb[3][3][3], glm::mat4 mat_four) {
    glm::bvec4 ao {false};
    glm::mat3 M = glm::inverse(glm::mat3 {mat_four});

    glm::ivec3 tl {glm::floor(glm::vec3(-1,-1,-1) * M + glm::vec3(0.5f))};
    glm::ivec3 tm {glm::floor(glm::vec3( 0,-1,-1) * M + glm::vec3(0.5f))};
    glm::ivec3 tr {glm::floor(glm::vec3( 1,-1,-1) * M + glm::vec3(0.5f))};
    glm::ivec3 ml {glm::floor(glm::vec3(-1, 0,-1) * M + glm::vec3(0.5f))};
    glm::ivec3 mr {glm::floor(glm::vec3( 1, 0,-1) * M + glm::vec3(0.5f))};
    glm::ivec3 bl {glm::floor(glm::vec3(-1, 1,-1) * M + glm::vec3(0.5f))};
    glm::ivec3 bm {glm::floor(glm::vec3( 0, 1,-1) * M + glm::vec3(0.5f))};
    glm::ivec3 br {glm::floor(glm::vec3( 1, 1,-1) * M + glm::vec3(0.5f))};

    ao.x = nb[tl.x+1][tl.y+1][tl.z+1] || nb[tm.x+1][tm.y+1][tm.z+1] || nb[ml.x+1][ml.y+1][ml.z+1];
    ao.y = nb[tr.x+1][tr.y+1][tr.z+1] || nb[tm.x+1][tm.y+1][tm.z+1] || nb[mr.x+1][mr.y+1][mr.z+1];
    ao.z = nb[bl.x+1][bl.y+1][bl.z+1] || nb[bm.x+1][bm.y+1][bm.z+1] || nb[ml.x+1][ml.y+1][ml.z+1];
    ao.w = nb[br.x+1][br.y+1][br.z+1] || nb[bm.x+1][bm.y+1][bm.z+1] || nb[mr.x+1][mr.y+1][mr.z+1];

    return ao;
}

mesh generic_plane(bool nb[3][3][3], glm::mat4 M, unsigned int type) {
    mesh m;
    glm::bvec4 ao = calc_ambient_occlusion(nb, M);

    m.tri_list.push_back(tri {{-0.5f, 0.5f,-0.5f, ao.z, 0, 1}, {0.5f, 0.5f,-0.5f, ao.w, 1, 1}, { 0.5f,-0.5f,-0.5f, ao.y, 1, 0}});
    m.tri_list.push_back(tri {{-0.5f, 0.5f,-0.5f, ao.z, 0, 1}, {0.5f,-0.5f,-0.5f, ao.y, 1, 0}, {-0.5f,-0.5f,-0.5f, ao.x, 0, 0}});

    m.tri_list[0].block_type_index = m.tri_list[1].block_type_index = type;
    m = m.transform(glm::translate(glm::mat4 {1.0f}, glm::vec3(0.5f)) * M);
    m.tri_list[0].world_normal = m.tri_list[1].world_normal = m.tri_list[0].calc_normal();

    return m;
}

mesh left_plane(bool nb[3][3][3], unsigned int type) {
    glm::mat4 M = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0,-1, 0));
    return generic_plane(nb, M, type);
}
mesh right_plane(bool nb[3][3][3], unsigned int type) {
    glm::mat4 M = glm::rotate(glm::mat4(1.0f), glm::radians( 90.0f), glm::vec3(0,-1, 0));
    return generic_plane(nb, M, type);
}
mesh top_plane(bool nb[3][3][3], unsigned int type) {
    glm::mat4 M = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
    return generic_plane(nb, M, type);
}
mesh bottom_plane(bool nb[3][3][3], unsigned int type) {
    glm::mat4 M = glm::rotate(glm::mat4(1.0f), glm::radians( 90.0f), glm::vec3(1, 0, 0));
    return generic_plane(nb, M, type);
}
mesh front_plane(bool nb[3][3][3], unsigned int type) {
    glm::mat4 M {1.0f};
    return generic_plane(nb, M, type);
}
mesh back_plane(bool nb[3][3][3], unsigned int type) {
    glm::mat4 M = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0,-1, 0));
    return generic_plane(nb, M, type);
}

} /* end of namespace tc::mesh_util */

#endif /* end of include guard: MESH_UTIL_HPP */
