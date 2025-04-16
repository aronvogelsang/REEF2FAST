#ifndef COMMON_HPP
#define COMMON_HPP

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

struct InterpolatedWavefield;  // Forward declaration

inline double round_to(double value, double precision = 1e-7) {
    return std::round(value / precision) * precision;
}

std::string find_wavefield_file(const std::string& directory);

/**
 * Represents a single data point in the wavefield.
 * All coordinates and field values are in OpenFAST convention.
 */
/*struct DataPoint {
    int timestep;
    double x, y, z;
    double vx, vy, vz;
    double ax, ay, az;
    double pressure;
    double elevation;
};*/


double read_z_max(const std::string& filename);

std::string format_scientific(double value);

/*std::vector<DataPoint> read_wavefield_csv(const std::string& filename);*/

bool read_wave_params(const std::string& filename, double& wave_dt, double& wave_tmax);

bool read_control_file(const std::string& filename, double& X_MIN, double& X_MAX, 
    double& Y_MIN, double& Y_MAX, double& Z_MIN, double& Z_MAX, 
    int& NX, int& NY, int& NZ);

bool write_elevation_csv(const InterpolatedWavefield& wf, const std::string& filename);

bool is_2D_case(const std::string& control_file);

std::vector<double> generate_linear_grid(double min_val, double max_val, int num_points);

bool read_timestep_from_control(const std::string& filename, double& delta_t);


#endif // COMMON_HPP