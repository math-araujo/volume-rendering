#include <cmath>
#include <iostream>
#include <stdexcept>

#include "primitives.hpp"
#include "ray.hpp"
#include "scene_tracer.hpp"
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
    const sf::Vector3f background{0.572f, 0.772f, 0.921f};
    if (!sphere.intersect(ray, record))
    {
        return background;
    }

    float step_size{0.2f};
    const int number_of_steps{static_cast<int>(std::ceil((record.max_root - record.min_root) / step_size))};
    step_size = (record.max_root - record.min_root) / number_of_steps;

    float transparency{1.0f};
    glm::vec3 final_color{0.0f, 0.0f, 0.0f};
    // For uniform ray-marching, the sample attenuation is assumed to be constant
    const float attenuation{volume::beer_lambert_transmittance(step_size, sphere.absorption_coeff)};
    for (int step = 0; step < number_of_steps; ++step)
    {
        float current_step_parameter{record.min_root + (step_size * (step + 0.5f))};
        const glm::vec3 sample_position{ray.evaluate(current_step_parameter)};
        transparency *= attenuation;
        const geometry::Ray in_scattering_ray{.origin = sample_position, .direction = light_direction};
        primitives::HitRecord volume_hit{.inside = false};

        // If the in-scattering ray is inside the volume, it will intersect the sphere
        // in the position where the ray entered the volume
        if (sphere.intersect(in_scattering_ray, volume_hit) && volume_hit.inside)
        {
            const float light_attenuation{
                volume::beer_lambert_transmittance(volume_hit.max_root, sphere.absorption_coeff)};
            const glm::vec3 in_scattering_contribution{light_attenuation * light_color}; // L_i(x)

            // Integration (Riemman Sum) of the sample
            final_color += (transparency * step_size * sphere.absorption_coeff) * in_scattering_contribution;
            /*
            NOTE: sphere.absorption_coeff on the equation/code above actually should be sphere.scattering_coefficient
            which wasn't presented on Chapter 2. Without the scattering term, a white sphere appears.
            I modified the original author's code and remove the scattering term, and it also resulted on a
            white sphere.
            I used sphere.absorption_coeff for simplicity (since the default values of absorption and scattering are
            equal) and to get a decent output, but the true term should be scattering, not absorption.
            */
        }
        else
        {
            throw std::runtime_error("In-scattering ray didn't intersect sphere");
        }
    }

    return (background * transparency) + sf::Vector3f{final_color.x, final_color.y, final_color.z};
}

} // namespace scene