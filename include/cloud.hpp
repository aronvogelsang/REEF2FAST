#pragma once

#include <vector>
#include <array>
#include <string>
#include <map>
#include <cmath>
#include "common.hpp"

struct WavefieldEntry {
    double x, y, z;             // Position
    double vx, vy, vz;          // Velocity
    double pressure;            // Pressure
    double elevation;           // Surface elevation


    // Acceleration – added later by computeAcceleration()
    double ax = NAN;
    double ay = NAN;
    double az = NAN;
};

struct InterpolatedWavefield {
    std::vector<std::array<double, 3>> grid_points;       // Target grid
    std::vector<std::vector<WavefieldEntry>> timesteps;   // Field values at each timestep
};

using Wavefield = std::vector<WavefieldEntry>;
using WavefieldByTime = std::map<int, Wavefield>;

struct InterpolationResult {
    InterpolatedWavefield interpolated;
    WavefieldByTime raw_data;
};


WavefieldByTime read_wavefield_csv(const std::string& filename, double z_max);
void generate_seastate_grid_targets(const std::string& control_file,
                                    std::vector<std::array<double, 3>>& targets);
std::vector<double> interpolate_to_grid(const Wavefield& wf,
                                        const std::vector<std::array<double, 3>>& target_pts,
                                        const std::string& field,
                                        int k = 8);
InterpolationResult interpolate_full_wavefield(const std::string& wavefield_file,
                                               const std::string& control_file);