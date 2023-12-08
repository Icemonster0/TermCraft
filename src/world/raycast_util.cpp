#include "raycast_util.hpp"

namespace tc::raycast_util {

Intersection::Intersection(glm::vec3 p_pos, glm::ivec3 p_block, glm::ivec3 p_block_offset, char p_axis)
    : pos(p_pos), block(p_block), block_offset(p_block_offset), axis(p_axis) {
}

std::list<Intersection> calc_ray_voxel_intersections(const glm::vec3 start, const glm::vec3 end, const float margin) {
    const glm::vec3 ray = end - start;
    const float ray_length = glm::length(ray);
    const glm::vec3 ray_dir = (ray_length == 0.0f) ? (glm::vec3(0.0f)) : (ray / ray_length);
    const glm::ivec3 dir_sign = glm::sign(ray_dir);

    const float inv_denom_x = 1.0f / ray.x;
    const float z_per_x = ray.z * inv_denom_x;
    const float y_per_x = ray.y * inv_denom_x;
    const float full_step_x = (ray.x == 0.0f)
                            ? std::numeric_limits<float>::max()
                            : sqrtf(1.0f + z_per_x * z_per_x + y_per_x * y_per_x);

    const float inv_denom_y = 1.0f / ray.y;
    const float x_per_y = ray.x * inv_denom_y;
    const float z_per_y = ray.z * inv_denom_y;
    const float full_step_y = (ray.y == 0.0f)
                            ? std::numeric_limits<float>::max()
                            : sqrtf(1.0f + x_per_y * x_per_y + z_per_y * z_per_y);

    const float inv_denom_z = 1.0f / ray.z;
    const float y_per_z = ray.y * inv_denom_z;
    const float x_per_z = ray.x * inv_denom_z;
    const float full_step_z = (ray.z == 0.0f)
                            ? std::numeric_limits<float>::max()
                            : sqrtf(1.0f + y_per_z * y_per_z + x_per_z * x_per_z);

    glm::ivec3 block {start};
    float next_step_x = full_step_x * ((dir_sign.x == -1) ? glm::fract(start.x) : 1.0f - glm::fract(start.x));
    float next_step_y = full_step_y * ((dir_sign.y == -1) ? glm::fract(start.y) : 1.0f - glm::fract(start.y));
    float next_step_z = full_step_z * ((dir_sign.z == -1) ? glm::fract(start.z) : 1.0f - glm::fract(start.z));

    std::list<Intersection> intersections;

    float length = 0.0f;
    while (true) {
        if (next_step_x < next_step_y && next_step_x < next_step_z) {
            length += next_step_x;
            if (length > ray_length) break;
            next_step_y -= next_step_x;
            next_step_z -= next_step_x;
            next_step_x = full_step_x;
            const glm::ivec3 block_offset = dir_sign * glm::ivec3(1, 0, 0);
            block += block_offset;
            intersections.emplace_back(start + ray_dir * length - margin * glm::vec3(block_offset), block, block_offset, 'X');
        }
        else if (next_step_y < next_step_x && next_step_y < next_step_z) {
            length += next_step_y;
            if (length > ray_length) break;
            next_step_x -= next_step_y;
            next_step_z -= next_step_y;
            next_step_y = full_step_y;
            const glm::ivec3 block_offset = dir_sign * glm::ivec3(0, 1, 0);
            block += block_offset;
            intersections.emplace_back(start + ray_dir * length - margin * glm::vec3(block_offset), block, block_offset, 'Y');
        }
        else {
            length += next_step_z;
            if (length > ray_length) break;
            next_step_x -= next_step_z;
            next_step_y -= next_step_z;
            next_step_z = full_step_z;
            const glm::ivec3 block_offset = dir_sign * glm::ivec3(0, 0, 1);
            block += block_offset;
            intersections.emplace_back(start + ray_dir * length - margin * glm::vec3(block_offset), block, block_offset, 'Z');
        }
    }

    return intersections;
}

} /* end of namespace tc::raycast_util */
