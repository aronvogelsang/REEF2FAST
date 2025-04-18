#pragma once

#include "structs.hpp"
#include <functional>
#include <string>
#include <vector>

/**
 * Streams a REEF3D wavefield CSV file (3D case).
 * Maintains a 3-timestep context and calls the provided callback.
 *
 * @param filename  Path to the CSV wavefield file
 * @param z_max     Maximum z-level from control.txt (used to adjust vertical reference)
 * @param callback  Function to process each timestep (prev, curr, next)
 */
void stream_wavefield_with_context(
    const std::string& filename,
    double z_max,
    std::function<void(int t,
                       const std::vector<WavefieldEntry>& prev,
                       const std::vector<WavefieldEntry>& curr,
                       const std::vector<WavefieldEntry>& next)> callback);

/**
 * Streams a REEF3D wavefield CSV file (2D case).
 * Filters the wavefield to a single y-slice and calls the callback with 3-timestep context.
 *
 * @param filename  Path to the CSV wavefield file
 * @param z_max     Maximum z-level from control.txt (used to adjust vertical reference)
 * @param callback  Function to process each timestep (prev, curr, next)
 */
void stream_wavefield_with_context_2d(
    const std::string& filename,
    double z_max,
    std::function<void(int t,
                       const std::vector<WavefieldEntry>& prev,
                       const std::vector<WavefieldEntry>& curr,
                       const std::vector<WavefieldEntry>& next)> callback);