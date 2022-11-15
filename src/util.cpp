#include "util.hpp"

namespace util
{

sf::Color vector_to_color(sf::Vector3f vector)
{
    vector.x = std::clamp(vector.x, 0.0f, 1.0f);
    vector.y = std::clamp(vector.y, 0.0f, 1.0f);
    vector.z = std::clamp(vector.z, 0.0f, 1.0f);
    return sf::Color{static_cast<std::uint8_t>(vector.x * 255.0f), static_cast<std::uint8_t>(vector.y * 255.0f),
                     static_cast<std::uint8_t>(vector.z * 255.0f)};
}

geometry::Ray transform_ray(const glm::mat4& matrix, glm::vec3 camera_origin, glm::vec3 pixel_position)
{
    glm::vec4 h_origin{camera_origin, 1.0f};
    glm::vec4 h_pixel{pixel_position, 1.0f};
    glm::vec3 origin_world{matrix * glm::vec4{camera_origin, 1.0f}};
    glm::vec3 pixel_world{matrix * glm::vec4{pixel_position, 1.0f}};
    glm::vec3 ray_dir = pixel_world - origin_world;
    return geometry::Ray{.origin = origin_world, .direction = glm::normalize(ray_dir)};
}

} // namespace util