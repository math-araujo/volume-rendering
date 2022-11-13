#ifndef DENSITY_HPP
#define DENSITY_HPP

#include <glm/glm.hpp>

// Forward declaration
namespace primitives
{

struct Box;

} // namespace primitives

namespace density
{

float eval_fbm(const glm::vec3& position, const glm::vec3& center, float radius);
float eval_grid(const glm::vec3& position, const primitives::Box& grid);

} // namespace density

#endif // DENSITY_HPP