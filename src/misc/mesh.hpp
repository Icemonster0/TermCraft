#ifndef MESH_HPP
#define MESH_HPP

#include "tri.hpp"

#include <vector>

namespace tc {

struct mesh {
    mesh(std::vector<tri> p_tri_list) : tri_list(p_tri_list) {
    }

    mesh() {
    }

    std::vector<tri> tri_list;
};

} /* end of namespace tc */

#endif /* end of include guard: MESH_HPP */
