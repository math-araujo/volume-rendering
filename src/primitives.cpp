#include <algorithm>

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

    float t0{(-linear_coeff - std::sqrt(discriminant)) / (2.0f * quadratic_coeff)};
    const float t1{(-linear_coeff + std::sqrt(discriminant)) / (2.0f * quadratic_coeff)};
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

    record.min_root = std::min(t0, t1);
    record.max_root = std::max(t0, t1);

    return true;
}

bool Box::intersect(const geometry::Ray& ray, HitRecord& record) const
{
    float root_x_min{(bounds[ray.sign[0]].x - ray.origin.x) * ray.inv_direction.x};
    float root_x_max{(bounds[1 - ray.sign[0]].x - ray.origin.x) * ray.inv_direction.x};
    float root_y_min{(bounds[ray.sign[1]].y - ray.origin.y) * ray.inv_direction.y};
    float root_y_max{(bounds[1 - ray.sign[1]].y - ray.origin.y) * ray.inv_direction.y};

    if (root_x_min > root_y_max || root_y_min > root_x_max)
    {
        return false;
    }

    if (root_y_min > root_x_min)
    {
        root_x_min = root_y_min;
    }

    if (root_y_max < root_x_max)
    {
        root_x_max = root_y_max;
    }

    float root_z_min{(bounds[ray.sign[2]].z - ray.origin.z) * ray.inv_direction.z};
    float root_z_max{(bounds[1 - ray.sign[2]].z - ray.origin.z) * ray.inv_direction.z};

    if (root_x_min > root_z_max || root_z_min > root_x_max)
    {
        return false;
    }

    /*if (root_z_min > root_x_min)
    {
        root_x_min = root_z_min;
    }
    if (root_z_max < root_x_max)
    {
        root_x_max = root_z_max;
    }*/

    record.min_root = std::max(root_z_min, root_x_min);
    record.max_root = std::min(root_z_max, root_x_max);
    return true;
}

} // namespace primitives