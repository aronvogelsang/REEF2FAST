// report_diagnostics.cpp
#include "report_diagnostics.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

void report_diagnostics_3d(const Wavefield& wf, int timestep) {
    double max_vx = 0.0, max_vy = 0.0, max_vz = 0.0;
    double max_ax = 0.0, max_ay = 0.0, max_az = 0.0;
    double max_p = 0.0, max_eta = 0.0;

    for (const auto& e : wf) {
        max_vx = std::max(max_vx, std::abs(e.vx));
        max_vy = std::max(max_vy, std::abs(e.vy));
        max_vz = std::max(max_vz, std::abs(e.vz));
        max_ax = std::max(max_ax, std::abs(e.ax));
        max_ay = std::max(max_ay, std::abs(e.ay));
        max_az = std::max(max_az, std::abs(e.az));
        max_p  = std::max(max_p,  std::abs(e.pressure));
        max_eta = std::max(max_eta, std::abs(e.elevation));
    }

    std::cout << "  Max |u|:    " << max_vx << "\n";
    std::cout << "  Max |v|:    " << max_vy << "\n";
    std::cout << "  Max |w|:    " << max_vz << "\n";
    std::cout << "  Max |ax|:   " << max_ax << "\n";
    std::cout << "  Max |ay|:   " << max_ay << "\n";
    std::cout << "  Max |az|:   " << max_az << "\n";
    std::cout << "  Max |p|:    " << max_p << "\n";
    std::cout << "  Max |eta|:  " << max_eta << "\n";
}

void report_diagnostics_2d(const Wavefield& wf, int timestep) {
    double max_vx = 0.0, max_vz = 0.0;
    double max_ax = 0.0, max_az = 0.0;
    double max_p = 0.0, max_eta = 0.0;

    for (const auto& e : wf) {
        max_vx = std::max(max_vx, std::abs(e.vx));
        max_vz = std::max(max_vz, std::abs(e.vz));
        max_ax = std::max(max_ax, std::abs(e.ax));
        max_az = std::max(max_az, std::abs(e.az));
        max_p  = std::max(max_p,  std::abs(e.pressure));
        max_eta = std::max(max_eta, std::abs(e.elevation));
    }

    std::cout << "  Max |u|:    " << max_vx << "\n";
    std::cout << "  Max |w|:    " << max_vz << "\n";
    std::cout << "  Max |ax|:   " << max_ax << "\n";
    std::cout << "  Max |az|:   " << max_az << "\n";
    std::cout << "  Max |p|:    " << max_p << "\n";
    std::cout << "  Max |eta|:  " << max_eta << "\n";
}

void report_grid_summary_3d(double X_MIN, double X_MAX,
                            double Y_MIN, double Y_MAX,
                            double Z_MIN, double Z_MAX,
                            int NX, int NY, int NZ,
                            size_t seastate_points) {
    std::cout << "\nREEF3D Grid Domain Size:\n";
    std::cout << "  X: from " << X_MIN << " to " << X_MAX << " (NX = " << NX << ")\n";
    std::cout << "  Y: from " << Y_MIN << " to " << Y_MAX << " (NY = " << NY << ")\n";
    std::cout << "  Z: from " << Z_MIN << " to " << Z_MAX << " (NZ = " << NZ << ")\n";
    std::cout << "  Total: "  << NX * NY * NZ << " grid points\n\n";

    std::cout << "OpenFAST SeaState grid created:\n";
    std::cout << "  X: from " << X_MIN << " to " << X_MAX << " (NX = " << NX - 1 << ")\n";
    std::cout << "  Y: from " << Y_MIN << " to " << Y_MAX << " (NY = " << NY - 1 << ")\n";
    std::cout << "  Z: from -" << Z_MAX << " to " << Z_MIN << " (NZ = " << NZ << ")\n";
    std::cout << "  Z grid stretching (cosine-based):\n";
    std::cout << "    z(i) = (cos(i * dthetaZ) - 1.0) * Z_Depth\n";
    std::cout << "    where dthetaZ = π / (NZ - 1)\n";
    std::cout << "    This clusters points near the free surface.\n";
    std::cout << "  Total: " << seastate_points << " grid points\n";
}

void report_grid_summary_2d(double X_MIN, double X_MAX,
                            double Z_MIN, double Z_MAX,
                            int NX, int NZ,
                            size_t seastate_points) {
    std::cout << "\n2D REEF3D Grid Domain (x–z plane):\n";
    std::cout << "  X: from " << X_MIN << " to " << X_MAX << " (NX = " << NX << ")\n";
    std::cout << "  Z: from " << Z_MIN << " to " << Z_MAX << " (NZ = " << NZ << ")\n";
    std::cout << "  Total: "  << NX * NZ << " grid points\n\n";

    std::cout << "2D SeaState grid created:\n";
    std::cout << "  X: from " << X_MIN << " to " << X_MAX << " (NX = " << NX - 1 << ")\n";
    std::cout << "  Z: from -" << Z_MAX << " to " << Z_MIN << " (NZ = " << NZ << ")\n";
    std::cout << "  Z grid stretching (cosine-based):\n";
    std::cout << "    z(i) = (cos(i * dthetaZ) - 1.0) * Z_Depth\n";
    std::cout << "    where dthetaZ = π / (NZ - 1)\n";
    std::cout << "    This clusters points near z = 0 (the free surface).\n";
    std::cout << "  Total: " << seastate_points << " grid points\n";
}