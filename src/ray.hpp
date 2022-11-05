#ifndef RAY_HPP
#define RAY_HPP

#include <glm/glm.hpp>

namespace geometry
{

struct Ray
{
    glm::vec3 origin{};
    glm::vec3 direction{};

    glm::vec3 evaluate(float parameter) const;
};

} // namespace geometry

#endif // RAY_HPP