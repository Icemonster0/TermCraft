#ifndef FRAG_SHADERS_HPP
#define FRAG_SHADERS_HPP

#include "../glm.hpp"

#include "../render/vertex.hpp"
#include "../render/buffer.hpp"
#include "../render/draw_util.hpp"
#include "../user_settings.hpp"
#include "../render/texture.hpp"
#include "../world/block.hpp"

#include <cmath>
#include <type_traits>

namespace tc {

struct frag_shaders {
    static glm::vec3 FRAG_default(fragment f, float global_time) {
        return glm::vec3(1.0f);
    }

    static glm::vec3 FRAG_fun(fragment f, float global_time) {
        float light = (glm::dot(face_world_normal(f), glm::normalize(glm::vec3(-0.7f, -1.0f, 0.4f)))*0.5f+0.5f) * 0.8f + 0.2f;
        // return glm::vec3(sin(interp_color(f).r*25)*0.5f+0.5f) * glm::vec3(sin(interp_color(f).b*25)*0.5f+0.5f);
        return glm::vec3(light);
        // return interp_color(f);
    }

    static glm::vec3 FRAG_shaded(fragment f, float global_time) {
        float light = glm::dot(face_world_normal(f), glm::normalize(glm::vec3(-0.7f, -1.0f, 0.4f)))*0.5f+0.5f;
        light = light * 0.3f + 0.7f;

        float ao = 1.0f - draw_util::square_interp(interp_ao(f));
        ao = ao * 0.2f + 0.8f;

        float fac = light * ao;

        float highlight = is_face_highlighted(f) ? 0.1f : 0.0f;

        float fog_begin = U.render_distance * (1.0f - U.fog);
        float fog = glm::clamp((1.0f / (U.render_distance - fog_begin)) * (interp_distance(f) - fog_begin), 0.0f, 1.0f);

        glm::vec3 albedo;
        if (U.bad_normals)
            albedo = glm::sign(face_view_normal(f).z) >= 0.0f ? glm::vec3 {1,0,0} : glm::vec3 {0,0,1};
        else {
            if (U.disable_textures) albedo = face_color(f);
            else albedo = sample_face_texture(f);
        }

        glm::vec3 block_color = albedo * fac + highlight;

        return glm::mix(block_color, U.sky_color, fog);
    }

private:

    static glm::vec4 interp_pos(fragment f) {
        return f.weights[0] * f.triangle->vertices[0].pos
             + f.weights[1] * f.triangle->vertices[1].pos
             + f.weights[2] * f.triangle->vertices[2].pos;
    }

    static float interp_distance(fragment f) {
        return f.weights[0] * f.triangle->vertices[0].distance
             + f.weights[1] * f.triangle->vertices[1].distance
             + f.weights[2] * f.triangle->vertices[2].distance;
    }

    static glm::vec2 interp_screenpos(fragment f) {
        return f.weights[0] * f.triangle->vertices[0].screenpos
             + f.weights[1] * f.triangle->vertices[1].screenpos
             + f.weights[2] * f.triangle->vertices[2].screenpos;
    }

    static glm::vec2 interp_tex_coord(fragment f) {
        return f.weights[0] * f.triangle->vertices[0].tex_coord
             + f.weights[1] * f.triangle->vertices[1].tex_coord
             + f.weights[2] * f.triangle->vertices[2].tex_coord;
    }

    static float interp_ao(fragment f) {
        return f.weights[0] * f.triangle->vertices[0].ao
             + f.weights[1] * f.triangle->vertices[1].ao
             + f.weights[2] * f.triangle->vertices[2].ao;
    }

    static glm::vec3 face_world_normal(fragment f) {
        return f.triangle->world_normal;
    }

    static glm::vec3 face_view_normal(fragment f) {
        return f.triangle->view_normal;
    }

    static glm::vec3 face_color(fragment f) {
        return (f.triangle->block_type_index < 0 ||
                f.triangle->block_type_index >= std::extent<decltype(block_type::block_color)>::value) ?
                block_type::block_color[0] :
                block_type::block_color[f.triangle->block_type_index];
    }

    static bool is_face_highlighted(fragment f) {
        return f.triangle->is_highlighted;
    }

    static glm::vec3 sample_face_texture(fragment f) {
        const Texture_Set *tex_set = (f.triangle->block_type_index < 0 ||
                                     f.triangle->block_type_index >= std::extent<decltype(block_type::block_texture)>::value) ?
                                     &block_type::block_texture[0] :
                                     &block_type::block_texture[f.triangle->block_type_index];
        return glm::vec3 {tex_set->sample(
            interp_tex_coord(f),
            f.triangle->block_side_index
        )};
    }
};

} /* end of namespace tc */

#endif /* end of include guard: FRAG_SHADERS_HPP */
