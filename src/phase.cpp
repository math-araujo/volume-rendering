#include <cmath>

#include "phase.hpp"

namespace phase
{

constexpr float pi()
{
    return 3.141592653589793238462643383279502884f;
}

float henyey_greenstein(float g, float cos_theta)
{
    const float base_denominator{1 + (g * g) - (2 * g * cos_theta)};
    return (1.0f / (4.0f * pi())) * ((1 - (g * g)) / (base_denominator * std::sqrt(base_denominator)));
}

} // namespace phase