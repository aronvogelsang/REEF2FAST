// elevation_geo.hpp
#pragma once

#include "cloud.hpp"
#include <string>

/**
 * Computes the surface elevation for each point in the interpolated wavefield.
 * Elevation is determined as the highest z-value at each unique (x, y) in the raw wavefield,
 * and then interpolated onto the target grid.
 *
 * @param wf The interpolated wavefield (will be updated in-place)
 * @param original_data The original (non-interpolated) REEF3D wavefield data
 * @param z_swl The still water level used as reference
 */
void compute_surface_elevation(InterpolatedWavefield& wf,
                               const WavefieldByTime& original_data
                               );