#ifndef SPLINE_HPP
#define SPLINE_HPP

#include "../glm.hpp"

namespace tc {

class Spline {
public:
    Spline(glm::vec3 p_end_a, glm::vec3 p_handle_a, glm::vec3 p_end_b, glm::vec3 p_handle_b);

    void cache_coefficients();
    glm::vec3 sample(float t);

private:
    glm::vec3 end_a, handle_a;
    glm::vec3 end_b, handle_b;

    glm::vec3 coeff1, coeff2, coeff3;
};

} /* end of namespace tc */

#endif /* end of include guard: SPLINE_HPP */
