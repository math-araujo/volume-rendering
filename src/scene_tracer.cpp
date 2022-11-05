#include "scene_tracer.hpp"
#include "primitives.hpp"
#include "ray.hpp"
#include "volume.hpp"

namespace scene
{

sf::Vector3f VolumeRayCasting::operator()(const geometry::Ray& ray, const primitives::Sphere& sphere) const
{
    float t0{0.0f};
    float t1{0.0f};
    const sf::Vector3f background{0.0f, 1.0f, 1.0f};
    if (sphere.intersect(ray, t0, t1))
    {
        const glm::vec3 first_hit{ray.evaluate(t0)};
        const glm::vec3 second_hit{ray.evaluate(t1)};
        const float distance{glm::length(second_hit - first_hit)};
        const float transmittance{volume::beer_lambert_transmittance(distance, sphere.absorption_coeff)};
        return volume::volume_scattering(transmittance, background, sphere.color);
    }

    return background;
}

} // namespace scene