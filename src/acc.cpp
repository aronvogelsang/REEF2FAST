//acc.cpp
#include "acc.hpp"
#include "common.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>



void computeAcceleration(InterpolatedWavefield& wf, double delta_t) {
    size_t grid_size = wf.grid_points.size();
    size_t total_timesteps = wf.timesteps.size();

    int computed = 0;

    for (size_t t = 0; t < total_timesteps; ++t) {
        for (size_t i = 0; i < grid_size; ++i) {
            WavefieldEntry& curr = wf.timesteps[t][i];
            if (t > 0 && t < total_timesteps - 1) {
                const WavefieldEntry& prev = wf.timesteps[t - 1][i];
                const WavefieldEntry& next = wf.timesteps[t + 1][i];
                curr.ax = (next.vx - prev.vx) / (2 * delta_t);
                curr.ay = (next.vy - prev.vy) / (2 * delta_t);
                curr.az = (next.vz - prev.vz) / (2 * delta_t);
            } else if (t == 0) {
                const WavefieldEntry& next = wf.timesteps[t + 1][i];
                curr.ax = (next.vx - curr.vx) / delta_t;
                curr.ay = (next.vy - curr.vy) / delta_t;
                curr.az = (next.vz - curr.vz) / delta_t;
            } else {
                const WavefieldEntry& prev = wf.timesteps[t - 1][i];
                curr.ax = (curr.vx - prev.vx) / delta_t;
                curr.ay = (curr.vy - prev.vy) / delta_t;
                curr.az = (curr.vz - prev.vz) / delta_t;
            }
            computed++;
        }
    }
    std::cout << "Computed acceleration for " << computed << " points.\n";
}


// // Currently unused – CSV writing now handled centrally in write_out_csv()
bool save_with_acceleration(const InterpolatedWavefield& wf, const std::string& filename) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Error: Could not write to " << filename << std::endl;
        return false;
    }

    out << "timestep,x,y,z,vx,vy,vz,pressure,elevation,ax,ay,az\n";

    for (size_t t = 0; t < wf.timesteps.size(); ++t) {
        const auto& timestep_data = wf.timesteps[t];
        for (size_t i = 0; i < wf.grid_points.size(); ++i) {
            const auto& pt = wf.grid_points[i];
            const auto& e = timestep_data[i];
            out << t << "," << pt[0] << "," << pt[1] << "," << pt[2] << ","
                << e.vx << "," << e.vy << "," << e.vz << ","
                << e.pressure << "," << e.elevation << ","
                << e.ax << "," << e.ay << "," << e.az << "\n";
        }
    }

    return true;
}
