#ifndef PRIMITIVES_HPP
#define PRIMITIVES_HPP

#include <array>
#include <vector>

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

struct Geometry
{
    virtual ~Geometry() = default;
    virtual bool intersect(const geometry::Ray& ray, HitRecord& record) const = 0;
};

struct Sphere : public Geometry
{
    float absorption_coeff{0.5f};
    float scattering_coeff{0.5f};
    float density{0.25f};
    float radius{5.0f};
    glm::vec3 center{0.0f, 0.0f, -20.0f};
    sf::Vector3f color{1.0f, 0.0f, 1.0f};

    bool intersect(const geometry::Ray& ray, HitRecord& record) const override;
};

struct Box : public Geometry
{
    bool intersect(const geometry::Ray& ray, HitRecord& record) const override;

    std::array<glm::vec3, 2> bounds{glm::vec3{-50.0f, -50.0f, -50.0f}, glm::vec3{50.0f, 50.0f, 50.0f}};
    float absorption_coeff{0.5f};
    float scattering_coeff{0.5f};
    int grid_resolution{128};
    std::vector<float> density;
};

using Grid = Box;

} // namespace primitives

#endif // PRIMITIVES_HPP