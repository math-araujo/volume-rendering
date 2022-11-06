#include "scene_tracer.hpp"
#include "primitives.hpp"
#include "ray.hpp"
#include "volume.hpp"

namespace scene
{

sf::Vector3f VolumeAbsorption::operator()(const geometry::Ray& ray, const primitives::Sphere& sphere) const
{
    primitives::HitRecord record{};
    const sf::Vector3f background{0.0f, 1.0f, 1.0f};
    if (sphere.intersect(ray, record))
    {
        const glm::vec3 first_hit{ray.evaluate(record.min_root)};
        const glm::vec3 second_hit{ray.evaluate(record.max_root)};
        const float distance{glm::length(second_hit - first_hit)};
        const float transmittance{volume::beer_lambert_transmittance(distance, sphere.absorption_coeff)};
        return volume::volume_scattering(transmittance, background, sphere.color);
    }

    return background;
}

sf::Vector3f VolumeInScattering::operator()(const geometry::Ray& ray, const primitives::Sphere& sphere) const
{
    primitives::HitRecord record{};
    const sf::Vector3f background{0.0f, 1.0f, 1.0f};
    if (sphere.intersect(ray, record))
    {
        return sf::Vector3f{1.0f, 0.0f, 0.0f};
    }

    return background;
}

} // namespace scene