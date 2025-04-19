#include "structs.hpp"
#include "streamingpipeline.hpp"
#include "wavefield_streaming.hpp"
#include "common.hpp"
#include "cloud.hpp"
#include "cloud2d.hpp"
#include "acc.hpp"
#include "acc2d.hpp"
#include "elevation_geo.hpp"
#include "elevation_geo2d.hpp"
#include "elevation_elev.hpp"
#include "elevation_elev2d.hpp"
#include "export.hpp"
#include "export_elevation.hpp"
#include "write_out.hpp"
#include "inflate2d.hpp"
#include "genSeaState.hpp"
#include "report_diagnostics.hpp"
#include <iostream>

StreamingPipeline::StreamingPipeline(const std::string& wavefield_file,
                                     const std::string& control_file,
                                     const std::string& ctrl_txt,
                                     bool is2D,
                                     const std::string& elevation_mode,
                                     bool write_csv,
                                     double y_total,
                                     int ny_usr)
    : wavefield_file(wavefield_file),
      control_file(control_file),
      ctrl_txt(ctrl_txt),
      is2D(is2D),
      elevation_mode(elevation_mode),
      write_csv(write_csv),
      y_total(y_total),
      ny_usr(ny_usr),
      grid_reported(false),
      seastate_written(false),
      first_elevation_written(false) {}

void StreamingPipeline::run() {
    // Generate target interpolation grid
    if (is2D) {
    generate_seastate_grid_targets_2d(control_file, target_grid);
    }else {
    generate_seastate_grid_targets(control_file, target_grid);
    }
    
    // Read grid bounds
    if (!read_control_file(control_file, X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX, NX, NY, NZ)) {
        throw std::runtime_error("Failed to read control.txt");
    }

    if (is2D) {
        Y_MIN = -y_total;
        Y_MAX =  y_total;
        NY = ny_usr;
    }

    // Optional console report
    if (!grid_reported) {
        if (is2D)
            report_grid_summary_2d(X_MIN, X_MAX, Z_MIN, Z_MAX, NX, NZ, target_grid.size());
        else
            report_grid_summary_3d(X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX, NX, NY, NZ, target_grid.size());
        grid_reported = true;
    }

    // Read wave parameters
    if (!read_wave_params(ctrl_txt, wave_dt, wave_tmax)) {
        throw std::runtime_error("Failed to read wave_dt and wave_tmax from ctrl.txt");
    }

    // Read max depth
    z_max = read_z_max(control_file);

    // Read Hs & Tp from ctrl.txt and write REEF2FAST.dat
    double wave_hs = 0.0, wave_tp = 0.0;
    if (!read_wave_parameters(ctrl_txt, wave_tmax, wave_dt, wave_hs, wave_tp)) {
        throw std::runtime_error("Failed to read Hs and Tp from ctrl.txt");
    }

    generate_seastate(X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX,
                      NX, NY, NZ, wave_tmax, wave_dt, wave_hs, wave_tp);
    seastate_written = true;

    std::cout << "\nStreaming and interpolating REEF3D wavefield...\n";

    if (is2D) {
        stream_wavefield_with_context_2d(wavefield_file, z_max,
            [&](int t,
                const std::vector<WavefieldEntry>& prev,
                const std::vector<WavefieldEntry>& curr,
                const std::vector<WavefieldEntry>& next) {
                process_timestep(t, prev, curr, next);
            });
    } else {
        stream_wavefield_with_context(wavefield_file, z_max,
            [&](int t,
                const std::vector<WavefieldEntry>& prev,
                const std::vector<WavefieldEntry>& curr,
                const std::vector<WavefieldEntry>& next) {
                process_timestep(t, prev, curr, next);
            });
    }

    std::cout << "\nAll timesteps processed successfully.\n";
}

void StreamingPipeline::process_timestep(int timestep,
    const std::vector<WavefieldEntry>& prev,
    const std::vector<WavefieldEntry>& curr,
    const std::vector<WavefieldEntry>& next) {

    std::cout << "\nTimestep: " << timestep << "\n";


    std::cout << "Target grid size: " << target_grid.size() << "\n";
    std::cout << "Curr wavefield size: " << curr.size() << "\n";
    std::cout << "Interpolating field: vx...\n";
    // Interpolation
    std::vector<double> vx, vy, vz, p;
    if (is2D) {
        vx = interpolate_to_grid_2d(curr, target_grid, "vx", 4);
        vy = std::vector<double>(target_grid.size(), 0.0);  // 2D: vy = 0
        vz = interpolate_to_grid_2d(curr, target_grid, "vz", 4);
        p  = interpolate_to_grid_2d(curr, target_grid, "pressure", 4);
    } else {
        vx = interpolate_to_grid(curr, target_grid, "vx", 4);
        vy = interpolate_to_grid(curr, target_grid, "vy", 4);
        vz = interpolate_to_grid(curr, target_grid, "vz", 4);
        p  = interpolate_to_grid(curr, target_grid, "pressure", 4);
    }

    // Build interpolated wavefield
    Wavefield interpolated;
    for (size_t i = 0; i < target_grid.size(); ++i) {
        const auto& pt = target_grid[i];
        interpolated.push_back(WavefieldEntry{
            pt[0], pt[1], pt[2],
            vx[i], vy[i], vz[i],
            p[i], NAN, NAN, NAN, NAN
        });
    }

    // Elevation & Acceleration
    if (is2D) {
        if (elevation_mode == "z") {
            compute_surface_elevation_geo_2d_single_timestep(interpolated, curr);
        } else {
            compute_surface_elevation_from_elev_2d_single_timestep(interpolated, curr);
        }
        computeAcceleration2D_from_context(prev, interpolated, next, wave_dt);
    } else {
        if (elevation_mode == "z") {
            compute_surface_elevation_geo_single_timestep(interpolated, curr);
        } else {
            compute_surface_elevation_from_elev_single_timestep(interpolated, curr);
        }
        computeAcceleration_from_context(prev, interpolated, next, wave_dt);
    }

    // Optional console diagnostics
    if (is2D) {
        report_diagnostics_2d(interpolated, timestep);
    } else {
        report_diagnostics_3d(interpolated, timestep);
    }

    // Inflate for 2D case
    if (is2D) {
        inflate_wavefield_y(interpolated, y_total, ny_usr);
    }

    // Export timestep to .XXX files
    bool append_wavefiles = (timestep > 0);
    generate_all_wavefiles(interpolated, wave_dt, timestep, append_wavefiles);

    // Export surface elevation streamingly
    bool append_elev = first_elevation_written;
    write_surface_elevation(interpolated, "REEF2FAST.Elev", wave_dt, timestep, append_elev);
    first_elevation_written = true;

    // Optional CSV
    if (write_csv) {
        bool append = (timestep > 0);
        write_out_csv(interpolated, "../output/interpolated_wavefield.csv", timestep, append);
    }
}