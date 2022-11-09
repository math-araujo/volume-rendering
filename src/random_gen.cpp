#include <random>

#include "random_gen.hpp"

namespace randomgen
{

float random_float()
{
    static std::default_random_engine generator;
    static std::uniform_real_distribution<float> distribution{0.0, 1.0};
    return distribution(generator);
}

float random_float(float min, float max)
{
    return min + ((max - min) * random_float());
}

} // namespace randomgen