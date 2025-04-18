#pragma once

#include <vector>
#include <array>
#include <string>
#include "structs.hpp"

// Grid generation for 2D SeaState
void generate_seastate_grid_targets_2d(const std::string& control_file,
                                       std::vector<std::array<double, 3>>& targets);

// Interpolation (2D x–z slice)
std::vector<double> interpolate_to_grid_2d(const Wavefield& wf,
                                           const std::vector<std::array<double, 3>>& target_pts,
                                           const std::string& field,
                                           int k = 4);