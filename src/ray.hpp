#ifndef RAY_HPP
#define RAY_HPP

#include <array>

#include <glm/glm.hpp>

namespace geometry
{

struct Ray
{
    glm::vec3 origin{};
    glm::vec3 direction{};
    glm::vec3 inv_direction{};
    std::array<bool, 3> sign{};

    glm::vec3 evaluate(float parameter) const;
    void compute_inv_direction();
};

} // namespace geometry

#endif // RAY_HPP