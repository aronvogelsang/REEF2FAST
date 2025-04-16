#pragma once

#include "cloud.hpp"
#include <string>

/**
 * Computes acceleration components ax and az in a 2D wavefield.
 * The y-direction (ay) is set to 0.0 since there is no variation in y.
 *
 * @param wf Interpolated wavefield data
 * @param delta_t Timestep used for central differences
 */
void computeAcceleration2D(InterpolatedWavefield& wf, double delta_t);