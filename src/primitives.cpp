#include "primitives.hpp"

#include "ray.hpp"

namespace primitives
{

bool Sphere::intersect(const geometry::Ray& ray, float& t0, float& t1) const
{
    const float quadratic_coeff{glm::dot(ray.direction, ray.direction)};
    const glm::vec3 center_to_origin{ray.origin - center};
    const float linear_coeff{2.0f * glm::dot(ray.direction, center_to_origin)};
    const float independent_coeff{glm::dot(center_to_origin, center_to_origin) - radius * radius};

    const float discriminant{linear_coeff * linear_coeff - 4 * quadratic_coeff * independent_coeff};
    if (discriminant < 0.0f)
    {
        return false;
    }

    t0 = (-linear_coeff - discriminant) / (2.0f * quadratic_coeff);
    t1 = (-linear_coeff + discriminant) / (2.0f * quadratic_coeff);
    return true;
}

} // namespace primitives