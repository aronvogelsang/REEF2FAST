#include "wheeler.hpp"
#include <cmath>
#include <algorithm> // for std::max
#include <iostream>

// Wheeler stretching for all points above SWL  (z > 0)
void apply_wheeler_stretching(std::vector<WavefieldEntry>& wavefield, double h) {
    int num_stretched = 0;

    for (auto& entry : wavefield) {
        if (entry.z > 0.0) {
            double eta = entry.elevation;

            // no division by 0
            double denom = h + eta;
            if (std::abs(denom) < 1e-6) {
                continue; // skip stretching for this point
            }

            double stretched_z = h * (h + entry.z) / denom - h;
            entry.z = stretched_z;
            ++num_stretched; 
        }
    }

    // Debugging to look at Wheeler
    if (num_stretched > 0) {
        std::cout << "Wheeler applied his stretching to " << num_stretched
                  << " points (z > 0 && eta > 0)" << std::endl;
    }
}