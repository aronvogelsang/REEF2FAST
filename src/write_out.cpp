#include "write_out.hpp"
#include "common.hpp"
#include <fstream>
#include <iostream>

bool write_out_csv(const std::vector<WavefieldEntry>& timestep_data,
                   const std::string& filename,
                   int timestep,
                   bool append) {
    std::ofstream out;

    if (append) {
        out.open(filename, std::ios::app);
    } else {
        out.open(filename);
    }

    if (!out.is_open()) {
        std::cerr << "[write_out_csv] Error: Could not open " << filename << " for writing.\n";
        return false;
    }

    // Write header if not appending
    if (!append) {
        out << "timestep,x,y,z,vx,vy,vz,pressure,elevation,ax,ay,az\n";
    }

    for (const auto& e : timestep_data) {
        out << timestep << ","
            << e.x << "," << e.y << "," << e.z << ","
            << e.vx << "," << e.vy << "," << e.vz << ","
            << e.pressure << "," << e.elevation << ","
            << e.ax << "," << e.ay << "," << e.az << "\n";
    }

    out.close();
    return true;
}