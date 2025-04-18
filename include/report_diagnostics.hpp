#pragma once

#include "structs.hpp"
#include <vector>
#include <array>

/**
 * Reports per-timestep max values of velocity, acceleration, pressure and elevation.
 * Used for diagnostics and debugging.
 */
void report_diagnostics_3d(const Wavefield& wf, int timestep);
void report_diagnostics_2d(const Wavefield& wf, int timestep);

/**
 * Reports a summary of the REEF3D and SeaState grids.
 */
void report_grid_summary_3d(double X_MIN, double X_MAX,
                            double Y_MIN, double Y_MAX,
                            double Z_MIN, double Z_MAX,
                            int NX, int NY, int NZ,
                            size_t seastate_points);

void report_grid_summary_2d(double X_MIN, double X_MAX,
                            double Z_MIN, double Z_MAX,
                            int NX, int NZ,
                            size_t seastate_points);