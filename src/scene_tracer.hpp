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

enum class Chapters
{
    VolumeAbsorption = 0,
    VolumeInScatteringBackward = 1,
    NumberOfChapters = 2,
};

/*

Functors implementing the topic of each chapter,
one functor per chapter.

*/

struct SceneTracer
{
    virtual ~SceneTracer() = default;
    virtual sf::Vector3f operator()(const geometry::Ray& ray, const primitives::Sphere& sphere) const = 0;
};

// Chapter 1 - Ray Casting with Beer-Lambert Law, implementing Indirect Light Absorption
struct VolumeAbsorption : public SceneTracer
{
    ~VolumeAbsorption() override = default;
    sf::Vector3f operator()(const geometry::Ray& ray, const primitives::Sphere& sphere) const override;
};

// Chapter 2 - Ray Marching Algorithm, adding the contributions of Light In-Scattering
struct VolumeInScattering : public SceneTracer
{
    ~VolumeInScattering() override = default;
    sf::Vector3f operator()(const geometry::Ray& ray, const primitives::Sphere& sphere) const override;

    glm::vec3 light_direction{0.0f, 1.0f, 0.0f};
    glm::vec3 light_color{1.3f, 0.3f, 0.9f};
};

// Chapter 3 - Ray Marching, now considering light in-Scattering, out-Scattering and absorption
struct VolumeComplete : public SceneTracer
{
    ~VolumeComplete() override = default;
    sf::Vector3f operator()(const geometry::Ray& ray, const primitives::Sphere& sphere) const override;

    glm::vec3 light_direction{0.0f, 1.0f, 0.0f};
    glm::vec3 light_color{15.0f, 0.0f, 15.0f};
    float assymetry_factor{0.1f};
    float russian_roulette{0.5f};
};

// Chapter 4 - 3D Density Field using procedural noise to create heterogeneous volumes
struct VolumeDensityField : public SceneTracer
{
    ~VolumeDensityField() override = default;
    sf::Vector3f operator()(const geometry::Ray& ray, const primitives::Sphere& sphere) const override;

    const sf::Vector3f background{0.572f, 0.772f, 0.921f};
    glm::vec3 light_direction{-0.315798f, 0.719361f, 0.618702f};
    glm::vec3 light_color{20.0f, 20.0f, 20.0f};
    float assymetry_factor{0.0f};
    float russian_roulette{0.5f};
};

} // namespace scene

#endif // SCENE_TRACER_HPP