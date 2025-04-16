#include "acc2d.hpp"
#include <iostream>
#include <cmath>

void computeAcceleration2D(InterpolatedWavefield& wf, double delta_t) {
    size_t grid_size = wf.grid_points.size();
    size_t total_timesteps = wf.timesteps.size();

    int computed = 0;

    for (size_t t = 0; t < total_timesteps; ++t) {
        for (size_t i = 0; i < grid_size; ++i) {
            WavefieldEntry& curr = wf.timesteps[t][i];

            // ay is always zero in 2D
            curr.ay = 0.0;

            if (t > 0 && t < total_timesteps - 1) {
                const WavefieldEntry& prev = wf.timesteps[t - 1][i];
                const WavefieldEntry& next = wf.timesteps[t + 1][i];
                curr.ax = (next.vx - prev.vx) / (2 * delta_t);
                curr.az = (next.vz - prev.vz) / (2 * delta_t);
            } else if (t == 0) {
                const WavefieldEntry& next = wf.timesteps[t + 1][i];
                curr.ax = (next.vx - curr.vx) / delta_t;
                curr.az = (next.vz - curr.vz) / delta_t;
            } else {
                const WavefieldEntry& prev = wf.timesteps[t - 1][i];
                curr.ax = (curr.vx - prev.vx) / delta_t;
                curr.az = (curr.vz - prev.vz) / delta_t;
            }

            computed++;
        }
    }

    std::cout << "Computed 2D acceleration for " << computed << " points.\n";
}