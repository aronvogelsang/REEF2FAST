#include "common.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include "cloud.hpp"

namespace fs = std::filesystem;

/*Function to find the wavefield. It needs to be saved in ../data with .csv suffix.
  The header needs to look like this: timestep,vx,vy,vz,pressure,elevation,x,y,z*/

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


//reading just max depth
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
                    throw std::runtime_error("Error: Malformed line in " + filename + " for B 10 entry");
                }
                file.close();
                return Z_MAX;
            }
        }
    }

    file.close();
    throw std::runtime_error("Error: z_max (B 10) not found in " + filename);
}

// Scientific notation for output files. Like in the originals.
std::string format_scientific(double value) {
    std::ostringstream stream;
    stream << std::scientific << std::setprecision(4) << value;
    return stream.str();
}

// Wavefield data funciton

/*std::vector<DataPoint> read_wavefield_csv(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<DataPoint> data;

    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << std::endl;
        return data;
    }

    std::string line;
    std::getline(file, line); // Skip header line

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        DataPoint dp;
        char comma;

        // parse alle Felder in der Reihenfolge:
        // timestep, x, y, z, vx, vy, vz, pressure, elevation, ax, ay, az

        if (!(ss >> dp.timestep >> comma
                  >> dp.x >> comma >> dp.y >> comma >> dp.z >> comma
                  >> dp.vx >> comma >> dp.vy >> comma >> dp.vz >> comma
                  >> dp.pressure >> comma >> dp.elevation >> comma
                  >> dp.ax >> comma >> dp.ay >> comma >> dp.az)) {
            std::cerr << "Warning: Could not parse line:\n" << line << std::endl;
            continue;
        }

        data.push_back(dp);
    }

    file.close();
    return data;
}*/

// Reading timestep and total time from ctrl.txt
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
            if (param == 41) {
                iss >> wave_tmax;
            }
        } else if (key == "P") {
            int param;
            iss >> param;
            if (param == 30) {
                iss >> wave_dt;
            }
        }
    }

    file.close();
    return true;
}

// Reading grid form control.txt
bool read_control_file(const std::string& filename, double& X_MIN, double& X_MAX, 
    double& Y_MIN, double& Y_MAX, double& Z_MIN, double& Z_MAX, 
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

    file.close();
    return true;
}


// Currently unused – CSV writing now handled centrally in write_out_csv()
bool write_elevation_csv(const InterpolatedWavefield& wf, const std::string& filename) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Failed to open output file: " << filename << "\n";
        return false;
    }

    out << "timestep,x,y,z,vx,vy,vz,pressure,elevation\n";
    for (size_t t = 0; t < wf.timesteps.size(); ++t) {
        const auto& entries = wf.timesteps[t];
        for (size_t i = 0; i < wf.grid_points.size(); ++i) {
            const auto& pt = wf.grid_points[i];
            const auto& e = entries[i];
            out << t << "," << pt[0] << "," << pt[1] << "," << pt[2] << ","
                << e.vx << "," << e.vy << "," << e.vz << ","
                << e.pressure << "," << e.elevation << "\n";
        }
    }

    return true;
}

//To detect if wavefield is 2D
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

std::vector<double> generate_linear_grid(double min_val, double max_val, int num_points) {
    std::vector<double> grid;
    double step = (max_val - min_val) / (num_points - 1);
    for (int i = 0; i < num_points; i++) {
        grid.push_back(min_val + i * step);
    }
    return grid;
}

bool read_timestep_from_control(const std::string& filename, double& delta_t) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        int param_type;
        double value;

        iss >> key >> param_type;
        if (key == "P" && param_type == 30) {
            if (iss >> value) {
                delta_t = value;
                file.close();
                std::cout << "delta_t = " << delta_t << "\n";
                return true;
            }
        }
    }

    std::cerr << "Error: No valid timestep (P 30) found in " << filename << std::endl;
    return false;
}