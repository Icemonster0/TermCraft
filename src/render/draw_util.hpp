#ifndef DRAW_UTIL_HPP
#define DRAW_UTIL_HPP

#include "../glm.hpp"

#include <string>

using namespace std;

namespace tc::draw_util {

enum Col_Type {BG, FG};

string ansi_color_string(Col_Type type, glm::vec3 c) {
    // example: "\e[48;2;255;0;0m" (set bg to red)
    return string("\e[").append(type == BG ? "48" : "38")
                        .append(";2;")
                        .append(to_string(int(c.r*255.0f))).append(";")
                        .append(to_string(int(c.g*255.0f))).append(";")
                        .append(to_string(int(c.b*255.0f))).append("m");
}

string ansi_value_string(Col_Type type, float v) {
    // example: "\e[38;5;127m" (set fg to gray)
    return string("\e[").append(type == BG ? "48" : "38")
                        .append(";5;")
                        .append(to_string(int(v*255.0f))).append("m");
}

string ansi_clear_string() {
    // clear all formatting
    return "\e[0m";
}

} /* end of namespace tc::draw_util */

#endif /* end of include guard: DRAW_UTIL_HPP */
