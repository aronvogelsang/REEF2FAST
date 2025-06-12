// streamingpipeline.hpp

#ifndef STREAMINGPIPELINE_HPP
#define STREAMINGPIPELINE_HPP

#include <string>
#include <vector>
#include "structs.hpp"

class StreamingPipeline {
public:
    StreamingPipeline(const std::string& wavefield_file,
                      const std::string& control_file,
                      const std::string& ctrl_txt,
                      bool is2D,
                      const std::string& elevation_mode,
                      bool write_csv,
                      double y_total,
                      int ny_usr,
                      bool use_wheeler);

    void run();
    void process_timestep(int timestep,
                          const std::vector<WavefieldEntry>& prev,
                          const std::vector<WavefieldEntry>& curr,
                          const std::vector<WavefieldEntry>& next);

private:
    // Input
    std::string wavefield_file;
    std::string control_file;
    std::string ctrl_txt;

    // Settings
    bool is2D;
    std::string elevation_mode;
    bool write_csv;
    double y_total;
    int ny_usr;
    bool use_wheeler;

    // Grid
    double X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX;
    int NX, NY, NZ;
    double z_max;
    std::vector<std::array<double, 3>> target_grid;

    // Time
    double wave_dt;
    double wave_tmax;

    // Flags
    bool grid_reported;
    bool seastate_written;
    bool first_elevation_written;
};

#endif // STREAMINGPIPELINE_HPP