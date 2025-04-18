#pragma once

#include "structs.hpp"

/**
 * Computes the surface elevation (z-surface) for each point in the interpolated wavefield.
 * Elevation is determined as the highest z-value at each unique (x, y) in the raw REEF3D wavefield,
 * and interpolated onto the SeaState grid using inverse distance weighting.
 *
 * This is the "geometric" elevation mode ("z").
 *
 * @param target The interpolated wavefield at current timestep (modified in-place)
 * @param raw    The original REEF3D wavefield at the same timestep
 */
void compute_surface_elevation_geo_single_timestep(std::vector<WavefieldEntry>& target,
                                                   const std::vector<WavefieldEntry>& raw);