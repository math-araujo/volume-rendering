#include "ray.hpp"

namespace geometry
{

Ray::Ray(const glm::vec3& ray_origin, const glm::vec3& ray_direction) :
    origin{ray_origin}, direction{ray_direction}, inv_direction{1.0f / ray_direction}
{
    for (std::size_t i = 0; i < sign.size(); ++i)
    {
        sign[i] = (inv_direction[i] < 0.0f);
    }
}

glm::vec3 Ray::evaluate(float parameter) const
{
    return origin + (parameter * direction);
}

} // namespace geometry