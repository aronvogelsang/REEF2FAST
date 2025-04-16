#pragma once

#include <vector>
#include <array>
#include <string>
#include <map>
#include <cmath>
#include "cloud.hpp"

WavefieldByTime read_wavefield_csv_2d(const std::string& filename, double z_max);
void generate_seastate_grid_targets_2d(const std::string& control_file,
                                       std::vector<std::array<double, 3>>& targets);
std::vector<double> interpolate_to_grid_2d(const Wavefield& wf,
                                           const std::vector<std::array<double, 3>>& target_pts,
                                           const std::string& field,
                                           int k = 4);
InterpolationResult interpolate_full_wavefield_2d(const std::string& wavefield_file,
                                                  const std::string& control_file);