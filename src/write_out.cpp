// write_out.cpp
#include "common.hpp"
#include "cloud.hpp"
#include <fstream>
#include <iostream>

bool write_out_csv(const InterpolatedWavefield& wf, const std::string& filename) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Error: Could not open " << filename << " for writing." << std::endl;
        return false;
    }

    out << "timestep,x,y,z,u,v,w,pressure,eta,ax,ay,az\n";

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