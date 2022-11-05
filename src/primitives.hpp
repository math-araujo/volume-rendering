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

struct Sphere
{
    float absorption_coeff{0.1f};
    float radius{1.0f};
    glm::vec3 center{0.0f, 0.0f, -4.0f};
    sf::Vector3f color{1.0f, 0.0f, 1.0f};

    bool intersect(const geometry::Ray& ray, float& t0, float& t1) const;
};

} // namespace primitives

#endif // PRIMITIVES_HPP