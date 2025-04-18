#pragma once

#include "structs.hpp"

/**
 * Computes 2D acceleration (ax, az) for each point in the interpolated wavefield.
 * Assumes vy = ay = 0.0 for all entries.
 */
void computeAcceleration2D_from_context(
    const std::vector<WavefieldEntry>& prev,
    std::vector<WavefieldEntry>& curr,
    const std::vector<WavefieldEntry>& next,
    double delta_t
);