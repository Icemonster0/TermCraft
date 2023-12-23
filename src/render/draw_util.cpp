#include "draw_util.hpp"

using namespace std;

namespace tc::draw_util {

string ansi_color_string(Col_Type type, glm::vec3 c) {
    // example: "\e[48;2;255;0;0m" (set bg to red)
    return string("\e[").append(type == BG ? "48" : "38")
                        .append(";2;")
                        .append(to_string(int(clamp(c.r, 0.0f, 1.0f)*255.0f))).append(";")
                        .append(to_string(int(clamp(c.g, 0.0f, 1.0f)*255.0f))).append(";")
                        .append(to_string(int(clamp(c.b, 0.0f, 1.0f)*255.0f))).append("m");
}

string ansi_bw_color_string(Col_Type type, glm::vec3 c) {
    // example: "\e[38;5;127m" (set fg to gray)
    float avg = (c.r + c.g + c.b) * 0.3333f;
    int value = glm::mix(231.1f, 256.1f, clamp(avg, 0.0f, 1.0f));
    // the following lines are needed because of stupid ansi index order
    if (value == 231) value = 16; // white to black
    if (value == 256) value = 231; // light_gray+1 to white

    return string("\e[").append(type == BG ? "48" : "38")
                        .append(";5;")
                        .append(to_string(value)).append("m");
}

string ascii_bw_color_string(Col_Type type, glm::vec3 c) {
    // convert bw value to ascii character
    // string chars = " .,:+#@";
    string chars = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'.";


    float avg = (c.r + c.g + c.b) * 0.3333f;
    int value = clamp(1.0f - avg, 0.0f, 1.0f) * chars.length()-1;
    return string {chars[value]};
}

string auto_color_string(Col_Type type, glm::vec3 c) {
    if (U.color_mode == "COMPAT")
        return ansi_bw_color_string(type, c);
    else
        return ansi_color_string(type, c);
}

string ansi_clear_string() {
    // clear all formatting
    return "\e[0m";
}

/* method from here:
 * https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle */
float half_plane(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}
bool is_point_in_triangle(glm::vec2 pt, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3) {
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = half_plane(pt, v1, v2);
    d2 = half_plane(pt, v2, v3);
    d3 = half_plane(pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

/* reference: https://ceng2.ktu.edu.tr/~cakir/files/grafikler/Texture_Mapping.pdf */
float cc_signed_area(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
    // a-b-c has to be counter-clockwise
    return  glm::cross(glm::vec3(b, 0.0f) - glm::vec3(a, 0.0f), glm::vec3(c, 0.0f) - glm::vec3(a, 0.0f)).z;
}

bool is_tri_in_NDC(tri t) {
    const glm::vec3 a = t.vertices[0].pos;
    const glm::vec3 b = t.vertices[1].pos;
    const glm::vec3 c = t.vertices[2].pos;

    /* If no plane (NDC box side) exists which every vertex is outside of,
     * the triangle is in NDC space.
     * Refrased:
     * If for every plane there is at least one vertex inside of that plane,
     * the triangle is in NDC space.
     * Note that this is only an approximation. */
    return (
        (a.x <  1 || b.x <  1 || c.x <  1) &&
        (a.x > -1 || b.x > -1 || c.x > -1) &&
        (a.y <  1 || b.y <  1 || c.y <  1) &&
        (a.y > -1 || b.y > -1 || c.y > -1) &&
        (a.z <  1 || b.z <  1 || c.z <  1) &&
        (a.z >  0 || b.z >  0 || c.z >  0)
    );
}

/* following two functions from: https://stackoverflow.com/questions/71420930/random-number-generator-with-3-inputs */
int rotl32(int n, char k) {
    int a = n << k;
    int b = n >> (32 - k);
    return a | b;
}
int three_input_random(int x, int y, int z) {
    int a = x;
    int b = y;
    int c = z;

    b ^= rotl32(a + c, 7);
    c ^= rotl32(b + a, 9);
    a ^= rotl32(c + b, 18);
    b ^= rotl32(a + c, 7);
    c ^= rotl32(b + a, 9);
    a ^= rotl32(c + b, 18);
    b ^= rotl32(a + c, 7);
    c ^= rotl32(b + a, 9);
    a ^= rotl32(c + b, 18);

    return a + b + c + x + y + z;
}
glm::vec3 random_vector(glm::ivec3 seed) {
    std::mt19937 gen(three_input_random(seed.x, seed.y, seed.z));
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

    glm::vec3 v;
    v.x = dis(gen);
    v.y = dis(gen);
    v.z = dis(gen);
    return glm::normalize(v);
}
float perlin_noise(glm::vec3 p, int seed) {
    // determine 8 corners of voxel
    const glm::ivec3 tlf {int(p.x),   int(p.y),   int(p.z)};
    const glm::ivec3 trf {int(p.x)+1, int(p.y),   int(p.z)};
    const glm::ivec3 blf {int(p.x),   int(p.y)+1, int(p.z)};
    const glm::ivec3 brf {int(p.x)+1, int(p.y)+1, int(p.z)};
    const glm::ivec3 tlb {int(p.x),   int(p.y),   int(p.z)+1};
    const glm::ivec3 trb {int(p.x)+1, int(p.y),   int(p.z)+1};
    const glm::ivec3 blb {int(p.x),   int(p.y)+1, int(p.z)+1};
    const glm::ivec3 brb {int(p.x)+1, int(p.y)+1, int(p.z)+1};

    // generate random vectors for each corner
    const glm::vec3 tlf_vec {random_vector(glm::ivec3(seed) + tlf)};
    const glm::vec3 trf_vec {random_vector(glm::ivec3(seed) + trf)};
    const glm::vec3 blf_vec {random_vector(glm::ivec3(seed) + blf)};
    const glm::vec3 brf_vec {random_vector(glm::ivec3(seed) + brf)};
    const glm::vec3 tlb_vec {random_vector(glm::ivec3(seed) + tlb)};
    const glm::vec3 trb_vec {random_vector(glm::ivec3(seed) + trb)};
    const glm::vec3 blb_vec {random_vector(glm::ivec3(seed) + blb)};
    const glm::vec3 brb_vec {random_vector(glm::ivec3(seed) + brb)};

    // calculate value for each corner
    const float tlf_val {glm::dot(p - glm::vec3(tlf), tlf_vec)};
    const float trf_val {glm::dot(p - glm::vec3(trf), trf_vec)};
    const float blf_val {glm::dot(p - glm::vec3(blf), blf_vec)};
    const float brf_val {glm::dot(p - glm::vec3(brf), brf_vec)};
    const float tlb_val {glm::dot(p - glm::vec3(tlb), tlb_vec)};
    const float trb_val {glm::dot(p - glm::vec3(trb), trb_vec)};
    const float blb_val {glm::dot(p - glm::vec3(blb), blb_vec)};
    const float brb_val {glm::dot(p - glm::vec3(brb), brb_vec)};

    // interpolate the values
    const glm::vec3 smooth_coord {
        square_interp(glm::fract(p.x)),
        square_interp(glm::fract(p.y)),
        square_interp(glm::fract(p.z)),
    };
    const float val = glm::mix(
        glm::mix( // front
            glm::mix(tlf_val, trf_val, smooth_coord.x), // top
            glm::mix(blf_val, brf_val, smooth_coord.x), // bottom
            smooth_coord.y
        ),
        glm::mix( // back
            glm::mix(tlb_val, trb_val, smooth_coord.x), // top
            glm::mix(blb_val, brb_val, smooth_coord.x), // bottom
            smooth_coord.y
        ),
        smooth_coord.z
    );

    return val;
}
float fractal_perlin_noise(glm::vec3 p, float scale, int octaves, float roughness, float lacunarity, int seed) {
    float size = scale;
    float amp = 1.0f;
    float amp_sum = amp;
    float val = 0.0f;
    for (int o = 0; o < octaves; ++o) {
        val += perlin_noise(p * size, seed + o) * amp;
        amp_sum += amp;
        size *= lacunarity;
        amp *= roughness;
    }
    val /= amp_sum;
    return val;
}

} /* end of namespace tc::draw_util */
