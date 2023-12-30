#include "spline.hpp"

namespace tc {

Spline::Spline(glm::vec3 p_end_a, glm::vec3 p_handle_a, glm::vec3 p_end_b, glm::vec3 p_handle_b)
    : end_a(p_end_a), end_b(p_end_b), handle_a(p_handle_a), handle_b(p_handle_b) {
    cache_coefficients();
}

void Spline::cache_coefficients() {
    coeff1 = end_a * (-3.0f) + handle_a * ( 3.0f);
    coeff2 = end_a * ( 3.0f) + handle_a * (-6.0f) + handle_b * ( 3.0f);
    coeff3 = end_a * (-1.0f) + handle_a * ( 3.0f) + handle_b * (-3.0f) + end_b * ( 1.0f);
}

glm::vec3 Spline::sample(float t) {
    return end_a +
           coeff1 * t +
           coeff2 * t*t +
           coeff3 * t*t*t;
}

} /* end of namespace tc */
