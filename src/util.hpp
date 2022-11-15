#ifndef UTIL_HPP
#define UTIL_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector3.hpp>
#include <glm/glm.hpp>

#include "ray.hpp"

namespace util
{

sf::Color vector_to_color(sf::Vector3f vector);
geometry::Ray transform_ray(const glm::mat4& matrix, glm::vec3 camera_origin, glm::vec3 pixel_position);

} // namespace util

#endif // UTIL_HPP