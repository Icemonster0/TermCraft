#ifndef DRAW_UTIL_HPP
#define DRAW_UTIL_HPP

#include "../glm.hpp"
#include "tri.hpp"
#include "../user_settings.hpp"

#include <string>
#include <algorithm>
#include <cmath>

using namespace std;

namespace tc::draw_util {

enum Col_Type {BG, FG};

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
    glm::vec3 a = t.vertices[0].pos;
    glm::vec3 b = t.vertices[1].pos;
    glm::vec3 c = t.vertices[2].pos;

    /* If no plane (NDC box side) exists which every vertex is outside of,
     * the triangle is in NDC space. */
    return (
        (abs(a.x) < 1 || abs(b.x) < 1 || abs(c.x) < 1) &&
        (abs(a.y) < 1 || abs(b.y) < 1 || abs(c.y) < 1) &&
        (abs(a.z) < 1 || abs(b.z) < 1 || abs(c.z) < 1)
    );
}

// interpolations
template <typename T> static T square_interp(T x) {
    return x < 0.5f ? 2.0f*x*x : 1.0f-2.0f*(x-1.0f)*(x-1.0f);
}
template <typename T> static T cubic_interp(T x) {
    return x < 0.5f ? 4.0f*x*x*x : 4.0f*(x-1.0f)*(x-1.0f)*(x-1.0f)+1.0f;
}

} /* end of namespace tc::draw_util */

#endif /* end of include guard: DRAW_UTIL_HPP */
