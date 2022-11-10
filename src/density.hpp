#ifndef DENSITY_HPP
#define DENSITY_HPP

#include <glm/glm.hpp>

namespace density
{

float eval_fbm(const glm::vec3& position, const glm::vec3& center, float radius);

} // namespace density

#endif // DENSITY_HPP