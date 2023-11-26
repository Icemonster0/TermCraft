#include "texture.hpp"

namespace tc {

// Texture:

Texture::Texture(const std::string relative_path) {
    #ifdef BUILD_PATH
        std::string full_path = BUILD_PATH;
    #else
        std::string full_path = ".";
    #endif
    full_path.append("/").append(relative_path);

    unsigned char *data = stbi_load(full_path.c_str(), &size.x, &size.y, &real_channels, channels);

    if (!data) {
        pixels.emplace_back();
        pixels[0].emplace_back(1.0f, 0.0f, 1.0f);
        alpha.emplace_back();
        alpha[0].emplace_back(1.0f);
        size = {1, 1};
        real_channels = 0;
        return;
    }

    float inverse_char_max = 1.0f / 255.0f;

    for (int x = 0; x < size.x; ++x) {
        pixels.emplace_back();
        alpha.emplace_back();
        for (int y = 0; y < size.y; ++y) {
            pixels[x].push_back(glm::vec3 {
                static_cast<float>(data[y*size.x*channels + x*channels + 0]),
                static_cast<float>(data[y*size.x*channels + x*channels + 1]),
                static_cast<float>(data[y*size.x*channels + x*channels + 2])
            } * inverse_char_max);
            alpha[x].push_back(static_cast<float>(data[y*size.x*channels + x*channels + 3]) * inverse_char_max);
        }
    }

    stbi_image_free(data);
}

glm::vec4 Texture::sample(const glm::vec2 tex_coord) const {
    glm::ivec2 corrected_tex_coord {glm::clamp(int(tex_coord.x * size.x), 0, size.x-1),
                                    glm::clamp(int(tex_coord.y * size.y), 0, size.y-1)};
    return glm::vec4 {pixels[corrected_tex_coord.x][corrected_tex_coord.y], alpha[corrected_tex_coord.x][corrected_tex_coord.y]};
}

// Texture_Set:

Texture_Set::Texture_Set(const std::string path) {
    std::shared_ptr<Texture> t = std::make_shared<Texture>(Texture {path});
    textures[tex::LEFT] = t;
    textures[tex::RIGHT] = t;
    textures[tex::TOP] = t;
    textures[tex::BOTTOM] = t;
    textures[tex::FRONT] = t;
    textures[tex::BACK] = t;
}

Texture_Set::Texture_Set(const std::string path_side, const std::string path_top_bottom) {
    std::shared_ptr<Texture> ta = std::make_shared<Texture>(Texture {path_side});
    std::shared_ptr<Texture> tb = std::make_shared<Texture>(Texture {path_top_bottom});
    textures[tex::LEFT] = ta;
    textures[tex::RIGHT] = ta;
    textures[tex::TOP] = tb;
    textures[tex::BOTTOM] = tb;
    textures[tex::FRONT] = ta;
    textures[tex::BACK] = ta;
}

Texture_Set::Texture_Set(const std::string path_side, const std::string path_top, const std::string path_bottom) {
    std::shared_ptr<Texture> ta = std::make_shared<Texture>(Texture {path_side});
    std::shared_ptr<Texture> tb = std::make_shared<Texture>(Texture {path_top});
    std::shared_ptr<Texture> tc = std::make_shared<Texture>(Texture {path_bottom});
    textures[tex::LEFT] = ta;
    textures[tex::RIGHT] = ta;
    textures[tex::TOP] = tb;
    textures[tex::BOTTOM] = tc;
    textures[tex::FRONT] = ta;
    textures[tex::BACK] = ta;
}

glm::vec4 Texture_Set::sample(const glm::vec2 tex_coord, const unsigned int side) const {
    return textures[side]->sample(tex_coord);
}

} /* end of namespace tc */
