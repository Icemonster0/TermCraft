#ifndef RAYCAST_UTIL_HPP
#define RAYCAST_UTIL_HPP

#include "../glm.hpp"

#include <list>
#include <cmath>
#include <limits>

namespace tc::raycast_util {

struct Intersection {
    Intersection(glm::vec3 p_pos, glm::ivec3 p_block, char p_axis);

    glm::vec3 pos;
    glm::ivec3 block;
    char axis;
};

std::list<Intersection> calc_ray_voxel_intersections(const glm::vec3 start, const glm::vec3 end);

} /* end of namespace tc::raycast_util */

#endif /* end of include guard: RAYCAST_UTIL_HPP */
