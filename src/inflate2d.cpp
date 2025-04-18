#include "inflate2d.hpp"
#include <vector>
#include <cmath>
#include <stdexcept>

void inflate_wavefield_y(Wavefield& wf, double Y_total, int NY) {
    if (NY < 2) {
        throw std::runtime_error("[inflate_wavefield_y] NY must be >= 2");
    }

    const int NSLICES = NY - 1;  // OpenFAST expects NY-1 slices
    const double dy = Y_total / NSLICES;
    const double Y_MIN = -Y_total / 2.0;

    Wavefield inflated;
    inflated.reserve(wf.size() * NSLICES);

    for (int j = 0; j < NSLICES; ++j) {
        double y_val = Y_MIN + (j + 0.5) * dy;

        for (const auto& entry : wf) {
            WavefieldEntry clone = entry;
            clone.y = y_val;
            inflated.push_back(clone);
        }
    }

    wf = std::move(inflated);
}