#pragma once

#include "structs.hpp"

/**
 * Interpolates surface elevation onto the target grid for one timestep.
 * Elevation is extracted from the REEF3D elevation values (eta) by taking
 * the maximum at each (x, y) location and interpolating onto the SeaState grid.
 *
 * @param target Interpolated wavefield (to be updated)
 * @param raw    Raw REEF3D wavefield data at current timestep
 */
void compute_surface_elevation_from_elev_single_timestep(
    std::vector<WavefieldEntry>& target,
    const std::vector<WavefieldEntry>& raw);