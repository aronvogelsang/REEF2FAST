#pragma once

#include <string>
#include <vector>

// Utility: Rounds to nearest multiple of precision (default 1e-7)
inline double round_to(double value, double precision = 1e-7) {
    return std::round(value / precision) * precision;
}

// Finds the first CSV wavefield file in the given directory
std::string find_wavefield_file(const std::string& directory);

// Reads z_max (maximum z value) from control.txt (B 10 block)
double read_z_max(const std::string& filename);

// Reads wave_dt and wave_tmax from ctrl.txt
bool read_wave_params(const std::string& filename, double& wave_dt, double& wave_tmax);

// Reads full grid definition from control.txt (B 2 and B 10)
bool read_control_file(const std::string& filename,
                       double& X_MIN, double& X_MAX,
                       double& Y_MIN, double& Y_MAX,
                       double& Z_MIN, double& Z_MAX,
                       int& NX, int& NY, int& NZ);

// Determines if case is 2D by checking if NY == 1 in control.txt
bool is_2D_case(const std::string& control_file);

// Formats a double into scientific notation for SeaState output
std::string format_scientific(double value);

// Utility: Generates a linearly spaced grid with N points
std::vector<double> generate_linear_grid(double min_val, double max_val, int num_points);