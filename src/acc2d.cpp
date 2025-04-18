#include "acc2d.hpp"
#include "structs.hpp"
#include <cmath>

void computeAcceleration2D_from_context(
    const std::vector<WavefieldEntry>& prev,
    std::vector<WavefieldEntry>& curr,
    const std::vector<WavefieldEntry>& next,
    double delta_t
) {
    for (size_t i = 0; i < curr.size(); ++i) {
        curr[i].ay = 0.0;

        if (!prev.empty() && !next.empty()) {
            curr[i].ax = (next[i].vx - prev[i].vx) / (2 * delta_t);
            curr[i].az = (next[i].vz - prev[i].vz) / (2 * delta_t);
        } else if (!next.empty()) {
            curr[i].ax = (next[i].vx - curr[i].vx) / delta_t;
            curr[i].az = (next[i].vz - curr[i].vz) / delta_t;
        } else if (!prev.empty()) {
            curr[i].ax = (curr[i].vx - prev[i].vx) / delta_t;
            curr[i].az = (curr[i].vz - prev[i].vz) / delta_t;
        }
    }
}