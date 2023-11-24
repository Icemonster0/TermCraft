#ifndef MESH_HPP
#define MESH_HPP

#include "tri.hpp"

#include <vector>

namespace tc {

struct tri;

struct mesh {
    mesh(std::vector<tri> p_tri_list);
    mesh();

    void append(mesh m);

    mesh transform(glm::mat4 M);

    std::vector<tri> tri_list;
};

} /* end of namespace tc */

#endif /* end of include guard: MESH_HPP */
