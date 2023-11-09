#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "../glm.hpp"
#include "../../lib/stb/stb_image.h"

#include <string>
#include <vector>

namespace tc {

class Texture {
public:
    Texture(const std::string path) {
        unsigned char *data = stbi_load(path.c_str(), &size.x, &size.y, &real_channels, channels);

        if (!data) {
            pixels.emplace_back();
            pixels[0].emplace_back(1.0f, 0.0f, 1.0f);
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

namespace tex {
    // refer to order in ../world/block.hpp
    const Texture textures[] {
        {"../res/tex/test.png"},
        {"../res/tex/grass_top.png"},
        {"../res/tex/dirt.png"},
        {"../res/tex/stone.png"},
    };
} /* end of namespace tex */

} /* end of namespace tc */

#endif /* end of include guard: TEXTURE_HPP */
