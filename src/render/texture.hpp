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
    Texture(const std::string path);

    glm::vec4 sample(const glm::vec2 tex_coord) const;

private:
    glm::ivec2 size {0};
    int real_channels = 0;
    const int channels = 4;
    std::vector<std::vector<glm::vec3>> pixels;
    std::vector<std::vector<float>> alpha;
};

class Texture_Set {
public:
    Texture_Set(const std::string path);
    Texture_Set(const std::string path_side, const std::string path_top_bottom);
    Texture_Set(const std::string path_side, const std::string path_top, const std::string path_bottom);

    glm::vec4 sample(const glm::vec2 tex_coord, const unsigned int side) const;

private:
    std::shared_ptr<Texture> textures[6];
};

} /* end of namespace tc */

#endif /* end of include guard: TEXTURE_HPP */
