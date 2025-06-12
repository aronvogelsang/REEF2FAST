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
#include "wheeler.hpp"
#include <iostream>

// The brain of the programme. The timestep-streaming architecture.

StreamingPipeline::StreamingPipeline(const std::string& wavefield_file,
                                     const std::string& control_file,
                                     const std::string& ctrl_txt,
                                     bool is2D,
                                     const std::string& elevation_mode,
                                     bool write_csv,
                                     double y_total,
                                     int ny_usr,
                                     bool use_wheeler)
    : wavefield_file(wavefield_file),
      control_file(control_file),
      ctrl_txt(ctrl_txt),
      is2D(is2D),
      elevation_mode(elevation_mode),
      write_csv(write_csv),
      y_total(y_total),
      ny_usr(ny_usr),
      use_wheeler(use_wheeler),
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

    // Optional: Wheeler-Stretching nur auf curr
    std::vector<WavefieldEntry> stretched_curr = curr;
    if (use_wheeler) {
        apply_wheeler_stretching(stretched_curr, z_max);
    }

    // Interpolation: prev, curr, next separat interpolieren
    Wavefield interp_prev, interp_curr, interp_next;

    if (is2D) {
        auto vx_p = interpolate_to_grid_2d(prev, target_grid, "vx", 4);
        auto vz_p = interpolate_to_grid_2d(prev, target_grid, "vz", 4);
        auto p_p  = interpolate_to_grid_2d(prev, target_grid, "pressure", 4);

        auto vx_c = interpolate_to_grid_2d(stretched_curr, target_grid, "vx", 4);
        auto vz_c = interpolate_to_grid_2d(stretched_curr, target_grid, "vz", 4);
        auto p_c  = interpolate_to_grid_2d(stretched_curr, target_grid, "pressure", 4);

        auto vx_n = interpolate_to_grid_2d(next, target_grid, "vx", 4);
        auto vz_n = interpolate_to_grid_2d(next, target_grid, "vz", 4);
        auto p_n  = interpolate_to_grid_2d(next, target_grid, "pressure", 4);

        for (size_t i = 0; i < target_grid.size(); ++i) {
            const auto& pt = target_grid[i];
            interp_prev.push_back({pt[0], pt[1], pt[2], vx_p[i], 0.0, vz_p[i], p_p[i], NAN, NAN, NAN, NAN});
            interp_curr.push_back({pt[0], pt[1], pt[2], vx_c[i], 0.0, vz_c[i], p_c[i], NAN, NAN, NAN, NAN});
            interp_next.push_back({pt[0], pt[1], pt[2], vx_n[i], 0.0, vz_n[i], p_n[i], NAN, NAN, NAN, NAN});
        }
    } else {
        auto vx_p = interpolate_to_grid(prev, target_grid, "vx", 4);
        auto vy_p = interpolate_to_grid(prev, target_grid, "vy", 4);
        auto vz_p = interpolate_to_grid(prev, target_grid, "vz", 4);
        auto p_p  = interpolate_to_grid(prev, target_grid, "pressure", 4);

        auto vx_c = interpolate_to_grid(stretched_curr, target_grid, "vx", 4);
        auto vy_c = interpolate_to_grid(stretched_curr, target_grid, "vy", 4);
        auto vz_c = interpolate_to_grid(stretched_curr, target_grid, "vz", 4);
        auto p_c  = interpolate_to_grid(stretched_curr, target_grid, "pressure", 4);

        auto vx_n = interpolate_to_grid(next, target_grid, "vx", 4);
        auto vy_n = interpolate_to_grid(next, target_grid, "vy", 4);
        auto vz_n = interpolate_to_grid(next, target_grid, "vz", 4);
        auto p_n  = interpolate_to_grid(next, target_grid, "pressure", 4);

        for (size_t i = 0; i < target_grid.size(); ++i) {
            const auto& pt = target_grid[i];
            interp_prev.push_back({pt[0], pt[1], pt[2], vx_p[i], vy_p[i], vz_p[i], p_p[i], NAN, NAN, NAN, NAN});
            interp_curr.push_back({pt[0], pt[1], pt[2], vx_c[i], vy_c[i], vz_c[i], p_c[i], NAN, NAN, NAN, NAN});
            interp_next.push_back({pt[0], pt[1], pt[2], vx_n[i], vy_n[i], vz_n[i], p_n[i], NAN, NAN, NAN, NAN});
        }
    }

    // Elevation only on curr (unstretched)
    if (is2D) {
        if (elevation_mode == "z") {
            compute_surface_elevation_geo_2d_single_timestep(interp_curr, curr);
        } else {
            compute_surface_elevation_from_elev_2d_single_timestep(interp_curr, curr);
        }
        computeAcceleration2D_from_context(interp_prev, interp_curr, interp_next, wave_dt);
    } else {
        if (elevation_mode == "z") {
            compute_surface_elevation_geo_single_timestep(interp_curr, curr);
        } else {
            compute_surface_elevation_from_elev_single_timestep(interp_curr, curr);
        }
        computeAcceleration_from_context(interp_prev, interp_curr, interp_next, wave_dt);
    }

    // Diagnostics
    if (is2D) report_diagnostics_2d(interp_curr, timestep);
    else      report_diagnostics_3d(interp_curr, timestep);

    // Inflate 2D
    if (is2D) inflate_wavefield_y(interp_curr, y_total, ny_usr);

    // Export
    bool append_wavefiles = (timestep > 0);
    generate_all_wavefiles(interp_curr, wave_dt, timestep, append_wavefiles);

    bool append_elev = first_elevation_written;
    write_surface_elevation(interp_curr, "REEF2FAST.Elev", wave_dt, timestep, append_elev);
    first_elevation_written = true;

    if (write_csv) {
        bool append = (timestep > 0);
        write_out_csv(interp_curr, "../output/interpolated_wavefield.csv", timestep, append);
    }
}