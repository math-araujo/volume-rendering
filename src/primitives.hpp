#ifndef PRIMITIVES_HPP
#define PRIMITIVES_HPP

#include <SFML/System/Vector3.hpp>
#include <glm/glm.hpp>

// Forward declaration
namespace geometry
{

struct Ray;

} // namespace geometry

namespace primitives
{

struct HitRecord
{
    float min_root{0.0f};
    float max_root{0.0f};
    bool inside{false};
};

struct Sphere
{
    float absorption_coeff{0.5f};
    float scattering_coeff{0.5f};
    float density{0.25f};
    float radius{5.0f};
    glm::vec3 center{0.0f, 0.0f, -20.0f};
    sf::Vector3f color{1.0f, 0.0f, 1.0f};

    bool intersect(const geometry::Ray& ray, HitRecord& record) const;
};

} // namespace primitives

#endif // PRIMITIVES_HPP