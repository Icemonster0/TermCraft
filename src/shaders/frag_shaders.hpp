#ifndef FRAG_SHADERS_HPP
#define FRAG_SHADERS_HPP

#include "../glm.hpp"

#include "../render/vertex.hpp"
#include "../render/buffer.hpp"

#include <cmath>

namespace tc {

struct frag_shaders {
    static glm::vec3 FRAG_default(fragment f, float global_time) {
        return glm::vec3(1.0f);
    }

    static glm::vec3 FRAG_fun(fragment f, float global_time) {
        return interp_color(f);
    }

private:
    static glm::vec3 interp_color(fragment f) {
        return f.weights[0] * f.triangle->vertices[0].color
             + f.weights[1] * f.triangle->vertices[1].color
             + f.weights[2] * f.triangle->vertices[2].color;
    }

    static glm::vec4 interp_pos(fragment f) {
        return f.weights[0] * f.triangle->vertices[0].pos
             + f.weights[1] * f.triangle->vertices[1].pos
             + f.weights[2] * f.triangle->vertices[2].pos;
    }

    static glm::vec2 interp_screenpos(fragment f) {
        return f.weights[0] * f.triangle->vertices[0].screenpos
             + f.weights[1] * f.triangle->vertices[1].screenpos
             + f.weights[2] * f.triangle->vertices[2].screenpos;
    }
};

} /* end of namespace tc */

#endif /* end of include guard: FRAG_SHADERS_HPP */
