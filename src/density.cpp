#include <algorithm>

#include <noise/noise.h>

#include "density.hpp"

namespace density
{

float eval_fbm(const glm::vec3& position, const glm::vec3& center, float radius)
{
    static noise::module::Perlin fbm;
    fbm.SetFrequency(0.5);
    fbm.SetOctaveCount(6);
    fbm.SetLacunarity(2.0);
    glm::vec3 relative_position = position - center;

    float distance = std::min(1.0f, relative_position.length() / radius);
    float falloff = glm::smoothstep(0.8f, 1.0f, distance);

    float fbm_result = static_cast<float>(fbm.GetValue(relative_position.x, relative_position.y, relative_position.z));
    return std::max(0.0f, fbm_result) * (1.0 - falloff);
}

} // namespace density