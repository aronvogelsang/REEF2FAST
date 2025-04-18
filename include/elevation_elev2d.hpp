#pragma once

#include "structs.hpp"

/**
 * Computes surface elevation (eta) for 2D x–z wavefields using REEF3D elevation values.
 * For each x-position, the maximum elevation value is used, then interpolated to grid.
 *
 * @param target Interpolated wavefield (updated in-place)
 * @param raw    Raw REEF3D wavefield data (same timestep)
 */
void compute_surface_elevation_from_elev_2d_single_timestep(
    std::vector<WavefieldEntry>& target,
    const std::vector<WavefieldEntry>& raw);