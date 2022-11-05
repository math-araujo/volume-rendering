#ifndef VOLUME_HPP
#define VOLUME_HPP

#include <SFML/System/Vector3.hpp>

namespace volume
{

float beer_lambert_transmittance(float distance, float absorption_coeff);

sf::Vector3f volume_scattering(float transmittance, const sf::Vector3f& background,
                               const sf::Vector3f& volume = sf::Vector3f{0.0f, 0.0f, 0.0f});

} // namespace volume

#endif // VOLUME_HPP