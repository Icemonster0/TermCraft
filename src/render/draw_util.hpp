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

string ansi_color_string(Col_Type type, glm::vec3 c);

string ansi_bw_color_string(Col_Type type, glm::vec3 c);

string ascii_bw_color_string(Col_Type type, glm::vec3 c);

string auto_color_string(Col_Type type, glm::vec3 c);

string ansi_clear_string();

float half_plane(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3);
bool is_point_in_triangle(glm::vec2 pt, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3);

float cc_signed_area(glm::vec2 a, glm::vec2 b, glm::vec2 c);

bool is_tri_in_NDC(tri t);

// interpolations
template <typename T> static T square_interp(T x) {
    return x < 0.5f ? 2.0f*x*x : 1.0f-2.0f*(x-1.0f)*(x-1.0f);
}
template <typename T> static T cubic_interp(T x) {
    return x < 0.5f ? 4.0f*x*x*x : 4.0f*(x-1.0f)*(x-1.0f)*(x-1.0f)+1.0f;
}

} /* end of namespace tc::draw_util */

#endif /* end of include guard: DRAW_UTIL_HPP */
