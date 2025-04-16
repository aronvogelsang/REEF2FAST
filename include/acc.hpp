#ifndef ACC_HPP
#define ACC_HPP

#include "cloud.hpp"
#include <string>


/**
 * Computes acceleration ax, ay, az for each point in the interpolated wavefield using central differences.
 */
void computeAcceleration(InterpolatedWavefield& result, double delta_t);

/**
 * Writes the interpolated wavefield (including acceleration) back to CSV.
 */
bool save_with_acceleration(const InterpolatedWavefield& result, const std::string& filename);

#endif // ACC_HPP
