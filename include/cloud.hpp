#pragma once

#include <vector>
#include <array>
#include <string>
#include "structs.hpp"

// Grid generation for 3D SeaState
void generate_seastate_grid_targets(const std::string& control_file,
                                    std::vector<std::array<double, 3>>& targets);

// Interpolation to target grid
std::vector<double> interpolate_to_grid(const Wavefield& wf,
                                        const std::vector<std::array<double, 3>>& target_pts,
                                        const std::string& field,
                                        int k = 8);