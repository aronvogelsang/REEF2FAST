#pragma once

#include "cloud.hpp"
#include <string>

/**
 * Computes the surface elevation for each point in the interpolated 2D wavefield.
 * Elevation is determined as the highest z-value at each unique x in the raw wavefield,
 * and then interpolated onto the target grid.
 *
 * @param wf The interpolated wavefield (will be updated in-place)
 * @param original_data The original (non-interpolated) REEF3D wavefield data
 */
void compute_surface_elevation_geo_2d(InterpolatedWavefield& wf,
                                      const WavefieldByTime& original_data);