#include "common.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <filesystem>


// Utilities

namespace fs = std::filesystem;

// --- Find wavefield CSV file ---
std::string find_wavefield_file(const std::string& directory) {
    std::vector<fs::path> csv_files;
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".csv") {
            csv_files.push_back(entry.path());
        }
    }

    if (csv_files.empty()) {
        throw std::runtime_error("No CSV file found in " + directory);
    } else if (csv_files.size() > 1) {
        std::cerr << "Multiple CSV files found:\n";
        for (const auto& path : csv_files) {
            std::cerr << "  - " << path.filename() << "\n";
        }
        throw std::runtime_error("Ambiguous input: please keep only one .csv in the data folder.");
    }

    std::cout << "Found wavefield file: " << csv_files[0].filename() << "\n";
    return csv_files[0].string();
}

// --- Read z_max from control.txt ---
double read_z_max(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Error: Could not open " + filename);
    }

    double X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX;
    int NX, NY, NZ;
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        iss >> key;
        if (key == "B") {
            int param_type;
            iss >> param_type;
            if (param_type == 10) {
                if (!(iss >> X_MIN >> X_MAX >> Y_MIN >> Y_MAX >> Z_MIN >> Z_MAX)) {
                    throw std::runtime_error("Error: Malformed B 10 line in " + filename);
                }
                return Z_MAX;
            }
        }
    }

    throw std::runtime_error("Error: z_max (B 10) not found in " + filename);
}

// --- Format value as scientific string ---
std::string format_scientific(double value) {
    std::ostringstream stream;
    stream << std::scientific << std::setprecision(4) << value;
    return stream.str();
}

// --- Read timestep and sim time from ctrl.txt ---
bool read_wave_params(const std::string& filename, double& wave_dt, double& wave_tmax) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        iss >> key;
        if (key == "N") {
            int param;
            iss >> param;
            if (param == 41) iss >> wave_tmax;
        } else if (key == "P") {
            int param;
            iss >> param;
            if (param == 30) iss >> wave_dt;
        }
    }

    return true;
}

// --- Read grid info from control.txt ---
bool read_control_file(const std::string& filename,
                       double& X_MIN, double& X_MAX,
                       double& Y_MIN, double& Y_MAX,
                       double& Z_MIN, double& Z_MAX,
                       int& NX, int& NY, int& NZ) {
    
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        iss >> key;
        if (key == "B") {
            int param_type;
            iss >> param_type;
            if (param_type == 2) {
                iss >> NX >> NY >> NZ;
            } else if (param_type == 10) {
                iss >> X_MIN >> X_MAX >> Y_MIN >> Y_MAX >> Z_MIN >> Z_MAX;
            }
        }
    }

    return true;
}

// --- Detect 2D wavefield (NY == 1) ---
bool is_2D_case(const std::string& control_file) {
    std::ifstream file(control_file);
    if (!file) {
        throw std::runtime_error("Error: Could not open " + control_file);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        iss >> key;
        if (key == "B") {
            int param_type;
            iss >> param_type;
            if (param_type == 2) {
                int NX, NY, NZ;
                iss >> NX >> NY >> NZ;
                return NY == 1;
            }
        }
    }

    throw std::runtime_error("Error: NY not found in control file (B 2 block missing).");
}

// --- Generate a linearly spaced grid ---
std::vector<double> generate_linear_grid(double min_val, double max_val, int num_points) {
    std::vector<double> grid;
    double step = (max_val - min_val) / (num_points - 1);
    for (int i = 0; i < num_points; i++) {
        grid.push_back(min_val + i * step);
    }
    return grid;
}