#ifndef MESH_UTIL_HPP
#define MESH_UTIL_HPP

#include "../glm.hpp"

#include "../render/mesh.hpp"
#include "block.hpp"
#include "../render/texture.hpp"

#include <cstdio>
#include <unistd.h>

namespace tc::mesh_util {

glm::bvec4 calc_ambient_occlusion(bool nb[3][3][3], glm::mat4 mat_four);

mesh generic_plane(bool nb[3][3][3], glm::mat4 M, unsigned int type, unsigned int side);

mesh left_plane(bool nb[3][3][3], unsigned int type);
mesh right_plane(bool nb[3][3][3], unsigned int type);
mesh top_plane(bool nb[3][3][3], unsigned int type);
mesh bottom_plane(bool nb[3][3][3], unsigned int type);
mesh front_plane(bool nb[3][3][3], unsigned int type);
mesh back_plane(bool nb[3][3][3], unsigned int type);

mesh diagonal_plane(bool nb[3][3][3], unsigned int type, bool flipped);

} /* end of namespace tc::mesh_util */

#endif /* end of include guard: MESH_UTIL_HPP */
