#include "ray.hpp"

namespace geometry
{

glm::vec3 Ray::evaluate(float parameter) const
{
    return origin + (parameter * direction);
}

} // namespace geometry