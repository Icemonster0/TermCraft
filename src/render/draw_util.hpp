#ifndef DRAW_UTIL_HPP
#define DRAW_UTIL_HPP

#include "../glm.hpp"

#include <string>
#include <algorithm>

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

string ansi_clear_string() {
    // clear all formatting
    return "\e[0m";
}

bool is_point_in_triangle(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p) {
    /* method from here:
     * https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle */

    float area = 0.5 *(-p1.y*p2.x + p0.y*(-p1.x + p2.x) + p0.x*(p1.y - p2.y) + p1.x*p2.y);
    float s = 1/(2*area)*(p0.y*p2.x - p0.x*p2.y + (p2.y - p0.y)*p.x + (p0.x - p2.x)*p.y);
    float t = 1/(2*area)*(p0.x*p1.y - p0.y*p1.x + (p0.y - p1.y)*p.x + (p1.x - p0.x)*p.y);

    return (s > 0 && t > 0 && 1-s-t > 0);
}

} /* end of namespace tc::draw_util */

#endif /* end of include guard: DRAW_UTIL_HPP */
