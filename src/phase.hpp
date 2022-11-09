#ifndef PHASE_HPP
#define PHASE_HPP

namespace phase
{

// g: asymmetry factor
// cos_theta: cosine of the angle between light direction and view direction
float henyey_greenstein(float g, float cos_theta);

} // namespace phase

#endif // PHASE_HPP