#include "volume.hpp"

#include <cmath>

namespace volume
{

float beer_lambert_transmittance(float distance, float absorption_coeff)
{
    return std::exp(-distance * absorption_coeff);
}

sf::Vector3f volume_scattering(float transmittance, const sf::Vector3f& background, const sf::Vector3f& volume)
{
    return (transmittance * background) + (1.0f - transmittance) * volume;
}

} // namespace volume
