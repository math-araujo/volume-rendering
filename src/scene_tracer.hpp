#ifndef SCENE_TRACER_HPP
#define SCENE_TRACER_HPP

#include <SFML/System/Vector3.hpp>
#include <glm/glm.hpp>

// Forward declarations
namespace geometry
{

class Ray;

} // namespace geometry

namespace primitives
{

class Sphere;

} // namespace primitives

namespace scene
{

struct SceneTracer
{
    virtual ~SceneTracer() = default;
    virtual sf::Vector3f operator()(const geometry::Ray& ray, const primitives::Sphere& sphere) const = 0;
};

// Chapter 1 - Ray Casting with Beer-Lambert Law
struct VolumeRayCasting : public SceneTracer
{
    ~VolumeRayCasting() override = default;

    sf::Vector3f operator()(const geometry::Ray& ray, const primitives::Sphere& sphere) const override;
};

} // namespace scene

#endif // SCENE_TRACER_HPP