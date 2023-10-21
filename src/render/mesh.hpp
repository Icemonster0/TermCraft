#ifndef MESH_HPP
#define MESH_HPP

#include "tri.hpp"

#include <vector>

namespace tc {

struct mesh {
    mesh(std::vector<tri> p_tri_list) : tri_list(p_tri_list) {
    }

    mesh() {}

    void append(mesh m) {
        tri_list.insert(std::end(tri_list), std::begin(m.tri_list), std::end(m.tri_list));
    }

    mesh transform(glm::mat4 M) {
        mesh m;
        for (tri t : tri_list) {
            for (vertex &v : t.vertices) {
                v.pos = M * v.pos;
            }
            m.tri_list.push_back(t);
        }
        return m;
    }

    std::vector<tri> tri_list;
};

} /* end of namespace tc */

#endif /* end of include guard: MESH_HPP */
