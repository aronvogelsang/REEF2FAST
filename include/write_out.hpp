#pragma once

#include "structs.hpp"  // For WavefieldEntry

/**
 * Writes a single timestep of the interpolated wavefield to a CSV file.
 * Each row corresponds to one point in the SeaState grid.
 *
 * @param timestep_data The current interpolated timestep data
 * @param filename      Output CSV path (e.g., ../output/interpolated_wavefield.csv)
 * @param timestep      Current timestep number
 * @param append        If true, appends to existing file; otherwise, overwrites
 * @return              True if file write was successful
 */
bool write_out_csv(const std::vector<WavefieldEntry>& timestep_data,
                   const std::string& filename,
                   int timestep,
                   bool append = false);