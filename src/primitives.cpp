#include "primitives.hpp"

#include "ray.hpp"

namespace primitives
{

bool Sphere::intersect(const geometry::Ray& ray, HitRecord& record) const
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

    /*record.min_root = (-linear_coeff - discriminant) / (2.0f * quadratic_coeff);
    record.max_root = (-linear_coeff + discriminant) / (2.0f * quadratic_coeff);*/
    float t0{(-linear_coeff - discriminant) / (2.0f * quadratic_coeff)};
    const float t1{(-linear_coeff + discriminant) / (2.0f * quadratic_coeff)};
    if (t0 < 0.0f)
    {
        if (t1 < 0.0f)
        {
            return false; // Sphere is completely behind the origin
        }
        else // Origin is inside the sphere
        {
            t0 = 0.0f;
            record.inside = true;
        }
    }

    record.min_root = t0;
    record.max_root = t1;

    return true;

    return true;
}

} // namespace primitives