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

    static glm::vec3 FRAG_shaded(fragment f, float global_time) {
        float light = glm::dot(face_normal(f), glm::normalize(glm::vec3(-0.7f, -1.0f, 0.4f)))*0.5f+0.5f;
        light = light * 0.3f + 0.7f;

        float ao = 1.0f - square_interp(interp_ao(f));
        ao = ao * 0.2f + 0.8f;

        return glm::vec3(light * ao);
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

    static float interp_ao(fragment f) {
        return f.weights[0] * f.triangle->vertices[0].ao
             + f.weights[1] * f.triangle->vertices[1].ao
             + f.weights[2] * f.triangle->vertices[2].ao;
    }

    static glm::vec3 face_normal(fragment f) {
        return f.triangle->normal;
    }

    template <typename T> static T square_interp(T x) {
        return x < 0.5f ? 2.0f*x*x : 1.0f-2.0f*(x-1.0f)*(x-1.0f);
    }

    template <typename T> static T cubic_interp(T x) {
        return x < 0.5f ? 4.0f*x*x*x : 4.0f*(x-1.0f)*(x-1.0f)*(x-1.0f)+1.0f;
    }
};

} /* end of namespace tc */

#endif /* end of include guard: FRAG_SHADERS_HPP */
