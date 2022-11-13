#include <algorithm>

#include <noise/noise.h>

#include "density.hpp"
#include "primitives.hpp"

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

float eval_grid(const glm::vec3& position, const primitives::Box& grid)
{
    const glm::vec3 grid_size{grid.bounds[1] - grid.bounds[0]};
    const glm::vec3 object_space_point{(position - grid.bounds[0]) / grid_size};
    const glm::vec3 voxel_space_point{static_cast<float>(grid.grid_resolution) * object_space_point};
    const glm::vec3 voxel_lattice{voxel_space_point - 0.5f};
    const int x{static_cast<int>(std::floor(voxel_lattice.x))};
    const int y{static_cast<int>(std::floor(voxel_lattice.y))};
    const int z{static_cast<int>(std::floor(voxel_lattice.z))};

    if (x < 0 || x >= grid.grid_resolution || y < 0 || y >= grid.grid_resolution || z < 0 || z >= grid.grid_resolution)
    {
        return 0.0f;
    }

    return grid.density[(z * grid.grid_resolution + y) * grid.grid_resolution + x];
    // return 1.0f;
}

} // namespace density