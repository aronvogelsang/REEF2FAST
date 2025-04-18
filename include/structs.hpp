#pragma once

#include <vector>
#include <array>
#include <cmath>

/**
 * Represents a single wavefield data point (interpolated or raw).
 * Includes velocity, pressure, elevation and (optional) acceleration.
 */
struct WavefieldEntry {
    double x, y, z;             // Position (in meters)
    double vx, vy, vz;          // Velocity components (m/s)
    double pressure;            // Dynamic pressure (Pa)
    double elevation;           // Surface elevation (m relative to SWL)

    // Acceleration (computed post-interpolation)
    double ax = NAN;
    double ay = NAN;
    double az = NAN;
};

/**
 * Represents a complete wavefield at a single timestep.
 */
using Wavefield = std::vector<WavefieldEntry>;