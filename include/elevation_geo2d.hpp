#pragma once

#include "structs.hpp"

/**
 * Computes the surface elevation (z-surface) for each point in a 2D wavefield (x–z plane).
 * Elevation is determined as the maximum z at each unique x in the raw REEF3D wavefield,
 * and interpolated onto the SeaState grid using inverse distance weighting.
 *
 * This is the "geometric" elevation mode ("z") for 2D cases.
 *
 * @param target The interpolated wavefield at the current timestep (modified in-place)
 * @param raw    The raw REEF3D wavefield at the same timestep
 */
void compute_surface_elevation_geo_2d_single_timestep(std::vector<WavefieldEntry>& target,
                                                      const std::vector<WavefieldEntry>& raw);