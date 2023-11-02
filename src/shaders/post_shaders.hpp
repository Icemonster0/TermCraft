#ifndef POST_SHADERS_HPP
#define POST_SHADERS_HPP

#include "../glm.hpp"

#include "../render/buffer.hpp"
#include "../render/draw_util.hpp"
#include "../user_settings.hpp"

#include <cmath>

namespace tc {

struct post_shaders {
    static glm::vec3 POST_default(const buffer<glm::vec3>* fbuf, const glm::ivec2 coord, const glm::ivec2 frame_size, const float global_time) {
        return fbuf->buf[coord.x][coord.y];
    }

    static glm::vec3 POST_vignette(const buffer<glm::vec3>* fbuf, const glm::ivec2 coord, const glm::ivec2 frame_size, const float global_time) {
        glm::vec2 norm_pos {(float)coord.x * 2.0f / (float)frame_size.x - 1.0f,
                            (float)coord.y * 2.0f / (float)frame_size.y - 1.0f};

        float dist = glm::length(norm_pos);
        float fac = draw_util::square_interp(glm::clamp(dist - 0.7f, 0.0f, 1.0f)) * 0.5f;

        return fbuf->buf[coord.x][coord.y] * (1.0f - fac);
    }
};

} /* end of namespace tc */

#endif /* end of include guard: POST_SHADERS_HPP */
