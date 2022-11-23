#include "ray.hpp"

namespace geometry
{

glm::vec3 Ray::evaluate(float parameter) const
{
    return origin + (parameter * direction);
}

void Ray::compute_inv_direction()
{
    inv_direction = 1.0f / direction;
    for (std::size_t i = 0; i < sign.size(); ++i)
    {
        sign[i] = (inv_direction[i] < 0.0f);
    }
}

} // namespace geometry