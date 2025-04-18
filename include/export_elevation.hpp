#pragma once

#include "structs.hpp"
#include <string>

/**
 * Writes surface elevation data at z ≈ 0 (SWL) to a SeaState-compatible .Elev file.
 * Elevation values are grouped by Y, sorted by X, and include metadata header.
 *
 * This function should be called per timestep with append = true for t > 0.
 */
void write_surface_elevation(const Wavefield& wf,
                             const std::string& filename,
                             double wave_dt,
                             int timestep,
                             bool append);