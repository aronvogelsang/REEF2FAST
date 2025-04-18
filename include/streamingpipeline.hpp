// StreamingPipeline.hpp
#pragma once

#include <string>
#include <vector>
#include <array>
#include "structs.hpp"  // Für WavefieldEntry etc.

/**
 * Main driver class for the REEF2FAST streaming pipeline.
 * Handles interpolation, elevation, acceleration and export,
 * all timestep-by-timestep using minimal memory.
 */
class StreamingPipeline {
public:
    StreamingPipeline(const std::string& wavefield_file,
                      const std::string& control_file,
                      const std::string& ctrl_txt,
                      bool is2D,
                      const std::string& elevation_mode,
                      bool write_csv,
                      double y_total,
                      int ny_usr);

    // Starts the streaming pipeline
    void run();

    private:
    // Processes a single timestep using 3-frame context
    void process_timestep(int timestep,
                          const std::vector<WavefieldEntry>& prev,
                          const std::vector<WavefieldEntry>& curr,
                          const std::vector<WavefieldEntry>& next);

    // Input paths and flags
    std::string wavefield_file;
    std::string control_file;
    std::string ctrl_txt;
    bool is2D;
    std::string elevation_mode;
    bool write_csv;
    double y_total;
    int ny_usr;

    // Global parameters
    double z_max;
    double wave_dt;
    double wave_tmax;

    // Grid parameters (needed for seastate and interpolation)
    double X_MIN, X_MAX;
    double Y_MIN, Y_MAX;
    double Z_MIN, Z_MAX;
    int NX, NY, NZ;

    // Interpolation target grid
    std::vector<std::array<double, 3>> target_grid;

    // Flags to track single-use operations
    bool seastate_written = false;
    bool grid_reported = false;
    bool first_elevation_written = false;
};
