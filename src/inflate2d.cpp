#include "cloud.hpp"
#include <vector>
#include <array>
#include <cmath>
#include <iostream>

/**
 * Inflate a 2D wavefield into a 3D wavefield by duplicating it in y-direction.
 *
 * The duplicated slices will lie at the centers of the cells defined by OpenFAST:
 * NY - 1 slices between [-Y_total/2, Y_total/2], equally spaced.
 *
 * @param wf       The 2D InterpolatedWavefield to be modified in-place
 * @param Y_total  Total y-domain width (e.g. 120.0 meters)
 * @param NY       Total number of y-grid points (as used in SeaState). NY-1 slices will be created.
 */
void inflate_wavefield_y(InterpolatedWavefield& wf, double Y_total, int NY)
{
    if (NY < 2) {
        std::cerr << "[inflate_wavefield_y] Error: NY must be at least 2.\n";
        return;
    }

    const int NSLICES = NY - 1;                 // OpenFAST expects NY-1 slices
    const double dy = Y_total / NSLICES;
    const double Y_MIN = -Y_total / 2.0;
    const double Y_MAX =  Y_total / 2.0;

    // Compute y-positions (center of each slice)
    std::vector<double> y_vals;
    for (int j = 0; j < NSLICES; ++j) {
        double y = Y_MIN + (j + 0.5) * dy;
        y_vals.push_back(y);
    }

    std::cout << "Inflating 2D wavefield along y-direction...\n";
    std::cout << "  NY = " << NY << " → " << NSLICES << " slices placed between "
              << Y_MIN << " and " << Y_MAX << " (dy = " << dy << ")\n";

    // Inflate grid points
    std::vector<std::array<double, 3>> new_grid;
    for (const auto& pt : wf.grid_points) {
        for (double y : y_vals) {
            new_grid.push_back({ pt[0], y, pt[2] });
        }
    }
    wf.grid_points = std::move(new_grid);

    // Inflate field values
    for (auto& timestep : wf.timesteps) {
        std::vector<WavefieldEntry> new_entries;
        for (const auto& e : timestep) {
            for (double y : y_vals) {
                WavefieldEntry copy = e;
                copy.y = y;
                new_entries.push_back(copy);
            }
        }
        timestep = std::move(new_entries);
    }

    std::cout << "Inflation complete. 3D Wavefield now contains " << wf.grid_points.size()
              << " grid points and " << wf.timesteps.size() << " timesteps.\n";
}