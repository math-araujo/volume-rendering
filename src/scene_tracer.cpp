#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include "density.hpp"
#include "phase.hpp"
#include "primitives.hpp"
#include "random_gen.hpp"
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

sf::Vector3f VolumeComplete::operator()(const geometry::Ray& ray, const primitives::Sphere& sphere) const
{
    primitives::HitRecord record;
    const sf::Vector3f background{0.572f, 0.772f, 0.921f};
    if (!sphere.intersect(ray, record))
    {
        return background;
    }

    float step_size{0.1f};
    const int number_of_steps{static_cast<int>(std::ceil((record.max_root - record.min_root) / step_size))};
    step_size = (record.max_root - record.min_root) / number_of_steps;

    glm::vec3 final_color{0.0f, 0.0f, 0.0f};
    float transparency{1.0f};
    const float attenuation{volume::beer_lambert_transmittance(
        step_size, sphere.density * (sphere.absorption_coeff + sphere.scattering_coeff))};
    for (int step = 0; step < number_of_steps; ++step)
    {
        // NOTE: when step = 0 and random_float returns a float close to zero,
        // the in-scattering ray doesn't intersect the sphere; the same hapens
        // when step = number_of_steps - 1 and random float returns a value that is close to 1.
        float jitter{randomgen::random_float(0.01f, 0.95f)};
        const float parameter{record.min_root + step_size * (step + jitter)};
        // const float parameter{record.min_root + step_size * (step + 0.5f)};
        const glm::vec3 sample_position{ray.evaluate(parameter)};
        transparency *= attenuation;

        const geometry::Ray in_scattering_ray{.origin = sample_position, .direction = light_direction};
        primitives::HitRecord volume_hit;
        if (sphere.intersect(in_scattering_ray, volume_hit) && volume_hit.inside)
        {
            const float light_attenuation{volume::beer_lambert_transmittance(
                volume_hit.max_root, sphere.density * (sphere.absorption_coeff + sphere.scattering_coeff))};
            const glm::vec3 in_scattering_contribution{light_attenuation * light_color};
            const float cos_theta{glm::dot(ray.direction, light_direction)};
            final_color += (phase::henyey_greenstein(assymetry_factor, cos_theta) * transparency * step_size *
                            sphere.scattering_coeff * sphere.density) *
                           in_scattering_contribution;
        }
        else
        {
            throw std::runtime_error("In-scattering ray didn't intersect sphere");
        }

        if (transparency < 1e-3)
        {
            if (randomgen::random_float() > russian_roulette)
            {
                break;
            }
            else
            {
                transparency *= (1.0f / russian_roulette);
            }
        }
    }

    return (background * transparency) + sf::Vector3f{final_color.x, final_color.y, final_color.z};
}

sf::Vector3f VolumeDensityField::operator()(const geometry::Ray& ray, const primitives::Sphere& sphere) const
{
    primitives::HitRecord record;
    if (!sphere.intersect(ray, record))
    {
        return background;
    }

    float step_size{0.1f};
    const int number_of_steps{static_cast<int>(std::ceil((record.max_root - record.min_root) / step_size))};
    step_size = (record.max_root - record.min_root) / number_of_steps;

    glm::vec3 final_color{0.0f, 0.0f, 0.0f};
    float transparency{1.0f};
    const float extinction_coeff{sphere.absorption_coeff + sphere.scattering_coeff};
    for (int step = 0; step < number_of_steps; ++step)
    {
        const float jitter{randomgen::random_float(0.01f, 0.95f)};
        const float parameter{record.min_root + step_size * (step + jitter)};
        const glm::vec3 sample_position{ray.evaluate(parameter)};

        const float density{density::eval_fbm(sample_position, sphere.center, sphere.radius)};
        const float attenuation{volume::beer_lambert_transmittance(step_size, density * extinction_coeff)};
        transparency *= attenuation;

        const geometry::Ray in_scattering_ray{.origin = sample_position, .direction = light_direction};
        primitives::HitRecord volume_hit;

        // Compute density along in-scattering light ray passing through heterogeneous volume using ray marching
        if (density > 0.0f && sphere.intersect(in_scattering_ray, volume_hit) && volume_hit.inside)
        {
            const int light_steps{static_cast<int>(std::ceil(volume_hit.max_root / step_size))};
            const float light_step_size{volume_hit.max_root / light_steps};
            float optical_depth{0.0f};
            for (int light_step = 0; light_step < light_steps; ++light_step)
            {
                const float light_parameter{light_step_size * (light_step + 0.5f)};
                const glm::vec3 light_sample_position{sample_position + (light_direction * light_parameter)};
                optical_depth += density::eval_fbm(light_sample_position, sphere.center, sphere.radius);
            }
            const float light_ray_attenuation{
                volume::beer_lambert_transmittance(light_step_size, optical_depth * extinction_coeff)};
            const glm::vec3 in_scattering_contribution{light_color * light_ray_attenuation};
            const float cos_theta{glm::dot(-ray.direction, light_direction)};
            final_color += in_scattering_contribution * phase::henyey_greenstein(assymetry_factor, cos_theta) *
                           sphere.scattering_coeff * transparency * step_size * density;
        }

        if (transparency < 1e-3)
        {
            if (randomgen::random_float() > russian_roulette)
            {
                break;
            }
            else
            {
                transparency /= russian_roulette;
            }
        }
    }

    return (background * transparency) + sf::Vector3f{final_color.x, final_color.y, final_color.z};
}

sf::Vector3f VolumeVoxelGrid::operator()(const geometry::Ray& ray, const primitives::Sphere& sphere) const
{
    primitives::HitRecord record;
    if (!sphere.intersect(ray, record))
    {
        return background;
    }

    return sf::Vector3f{1.0f, 0.0f, 0.0f};
}

sf::Vector3f VolumeVoxelGrid::operator()(const geometry::Ray& ray, const primitives::Box& box) const
{
    primitives::HitRecord record;
    if (!box.intersect(ray, record))
    {
        return background;
    }

    float step_size{0.1f};
    const int number_of_steps{static_cast<int>(std::ceil((record.max_root - record.min_root) / step_size))};
    step_size = (record.max_root - record.min_root) / number_of_steps;

    glm::vec3 final_color{0.0f, 0.0f, 0.0f};
    float transparency{1.0f};
    const float extinction_coeff{box.absorption_coeff + box.scattering_coeff};
    for (int step = 0; step < number_of_steps; ++step)
    {
        const float jitter{randomgen::random_float(0.01f, 0.95f)};
        const float parameter{record.min_root + step_size * (step + jitter)};
        const glm::vec3 sample_position{ray.evaluate(parameter)};

        const float density{density::eval_grid(sample_position, box)};
        const float attenuation{volume::beer_lambert_transmittance(step_size, density * extinction_coeff)};
        transparency *= attenuation;

        geometry::Ray in_scattering_ray{.origin = sample_position, .direction = light_direction};
        in_scattering_ray.compute_inv_direction();
        primitives::HitRecord volume_hit;

        // Compute density along in-scattering light ray passing through heterogeneous volume using ray marching
        if (density > 0.0f && box.intersect(in_scattering_ray, volume_hit) && volume_hit.max_root > 0.0f)
        {
            const int light_steps{static_cast<int>(std::ceil(volume_hit.max_root / step_size))};
            const float light_step_size{volume_hit.max_root / light_steps};
            float optical_depth{0.0f};
            for (int light_step = 0; light_step < light_steps; ++light_step)
            {
                const float light_parameter{light_step_size * (light_step + 0.5f)};
                const glm::vec3 light_sample_position{sample_position + (light_direction * light_parameter)};
                optical_depth += density::eval_grid(light_sample_position, box);
            }
            const float light_ray_attenuation{
                volume::beer_lambert_transmittance(light_step_size, optical_depth * extinction_coeff)};
            const glm::vec3 in_scattering_contribution{light_color * light_ray_attenuation};
            const float cos_theta{glm::dot(-ray.direction, light_direction)};
            final_color += in_scattering_contribution * phase::henyey_greenstein(assymetry_factor, cos_theta) *
                           box.scattering_coeff * transparency * step_size * density;
        }

        if (transparency < 1e-3)
        {
            if (randomgen::random_float() > russian_roulette)
            {
                break;
            }
            else
            {
                transparency /= russian_roulette;
            }
        }
    }

    return (background * transparency) + sf::Vector3f{final_color.x, final_color.y, final_color.z};
}

} // namespace scene