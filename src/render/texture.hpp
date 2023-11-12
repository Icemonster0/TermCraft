#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "../glm.hpp"
#include "../../lib/stb/stb_image.h"

#include <string>
#include <vector>
#include <memory>

namespace tc {

namespace tex {
    enum Side {
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,
        FRONT,
        BACK
    };
} /* end of namespace tex */

class Texture {
public:
    Texture(const std::string path) {
        unsigned char *data = stbi_load(path.c_str(), &size.x, &size.y, &real_channels, channels);

        if (!data) {
            pixels.emplace_back();
            pixels[0].emplace_back(1.0f, 0.0f, 1.0f);
            size = {1, 1};
            real_channels = 0;
            return;
        }

        float inverse_char_max = 1.0f / 255.0f;

        for (int x = 0; x < size.x; ++x) {
            pixels.emplace_back();
            for (int y = 0; y < size.y; ++y) {
                pixels[x].push_back(glm::vec3 {
                    static_cast<float>(data[y*size.x*channels + x*channels + 0]),
                    static_cast<float>(data[y*size.x*channels + x*channels + 1]),
                    static_cast<float>(data[y*size.x*channels + x*channels + 2])
                } * inverse_char_max);
            }
        }

        stbi_image_free(data);
    }

    glm::vec3 sample(const glm::vec2 tex_coord) const {
        return pixels[glm::clamp(int(tex_coord.x * size.x), 0, size.x-1)][glm::clamp(int(tex_coord.y * size.y), 0, size.y-1)];
    }

private:
    glm::ivec2 size {0};
    int real_channels = 0;
    const int channels = 3;
    std::vector<std::vector<glm::vec3>> pixels;
};

class Texture_Set {
public:
    Texture_Set(const std::string path) {
        std::shared_ptr<Texture> t = std::make_shared<Texture>(Texture {path});
        textures[tex::LEFT] = t;
        textures[tex::RIGHT] = t;
        textures[tex::TOP] = t;
        textures[tex::BOTTOM] = t;
        textures[tex::FRONT] = t;
        textures[tex::BACK] = t;
    }

    Texture_Set(const std::string path_side, const std::string path_top_bottom) {
        std::shared_ptr<Texture> ta = std::make_shared<Texture>(Texture {path_side});
        std::shared_ptr<Texture> tb = std::make_shared<Texture>(Texture {path_top_bottom});
        textures[tex::LEFT] = ta;
        textures[tex::RIGHT] = ta;
        textures[tex::TOP] = tb;
        textures[tex::BOTTOM] = tb;
        textures[tex::FRONT] = ta;
        textures[tex::BACK] = ta;
    }

    Texture_Set(const std::string path_side, const std::string path_top, const std::string path_bottom) {
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

    glm::vec3 sample(const glm::vec2 tex_coord, const unsigned int side) const {
        return textures[side]->sample(tex_coord);
    }

private:
    std::shared_ptr<Texture> textures[6];
};

} /* end of namespace tc */

#endif /* end of include guard: TEXTURE_HPP */
