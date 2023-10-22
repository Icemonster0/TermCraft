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
        float light = (glm::dot(face_normal(f), glm::normalize(glm::vec3(-0.7f, -1.0f, 0.4f)))*0.5f+0.5f) * 0.8f + 0.2f;
        // return glm::vec3(sin(interp_color(f).r*25)*0.5f+0.5f) * glm::vec3(sin(interp_color(f).b*25)*0.5f+0.5f);
        return glm::vec3(light);
        // return interp_color(f);
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

    static glm::vec3 face_normal(fragment f) {
        return f.triangle->normal;
    }
};

} /* end of namespace tc */

#endif /* end of include guard: FRAG_SHADERS_HPP */
