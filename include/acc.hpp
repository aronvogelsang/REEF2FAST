#pragma once

#include "structs.hpp"
#include <vector>

/**
 * Computes acceleration (ax, ay, az) for each point in the interpolated wavefield
 * using finite differences in time.
 *
 * Central difference is used when both previous and next timesteps are available.
 * Forward/backward difference is used at the edges.
 *
 * @param prev   Wavefield at t - Δt (can be empty)
 * @param curr   Wavefield at current timestep t (will be updated)
 * @param next   Wavefield at t + Δt (can be empty)
 * @param delta_t Time step size
 */
void computeAcceleration_from_context(
    const std::vector<WavefieldEntry>& prev,
    std::vector<WavefieldEntry>& curr,
    const std::vector<WavefieldEntry>& next,
    double delta_t
);