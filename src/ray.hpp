#ifndef RAY_HPP
#define RAY_HPP

#include <array>

#include <glm/glm.hpp>

namespace geometry
{

struct Ray
{
    Ray(const glm::vec3& ray_origin, const glm::vec3& ray_direction);
    glm::vec3 origin{};
    glm::vec3 direction{};
    glm::vec3 inv_direction{};
    std::array<bool, 3> sign{};

    glm::vec3 evaluate(float parameter) const;
};

} // namespace geometry

#endif // RAY_HPP