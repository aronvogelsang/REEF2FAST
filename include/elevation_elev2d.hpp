#pragma once

#include "cloud.hpp"
#include <string>

/**
 * Computes the surface elevation for each point in the interpolated 2D wavefield.
 * Elevation is determined based on the maximum elevation value at each unique x
 * in the raw wavefield (not based on z), and then interpolated onto the target grid.
 *
 * @param wf The interpolated wavefield (will be updated in-place)
 * @param original_data The original (non-interpolated) REEF3D wavefield data
 */
void compute_surface_elevation_from_elev_2d(InterpolatedWavefield& wf,
                                            const WavefieldByTime& original_data);