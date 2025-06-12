#include "acc.hpp"
#include "structs.hpp"
#include <cmath>
// fpr debudding can be deleted later
#include <iostream>

void computeAcceleration_from_context(
    const std::vector<WavefieldEntry>& prev,
    std::vector<WavefieldEntry>& curr,
    const std::vector<WavefieldEntry>& next,
    double delta_t)
{
    std::cout << "prev size: " << prev.size() 
              << ", curr size: " << curr.size() 
              << ", next size: " << next.size() << std::endl;

    const size_t n = curr.size();

    for (size_t i = 0; i < n; ++i) {
        if (!prev.empty() && !next.empty()) {
            // Central difference
            curr[i].ax = (next[i].vx - prev[i].vx) / (2.0 * delta_t);
            curr[i].ay = (next[i].vy - prev[i].vy) / (2.0 * delta_t);
            curr[i].az = (next[i].vz - prev[i].vz) / (2.0 * delta_t);
        } else if (!next.empty()) {
            // Forward difference (start of simulation)
            curr[i].ax = (next[i].vx - curr[i].vx) / delta_t;
            curr[i].ay = (next[i].vy - curr[i].vy) / delta_t;
            curr[i].az = (next[i].vz - curr[i].vz) / delta_t;
        } else if (!prev.empty()) {
            // Backward difference (end of simulation)
            curr[i].ax = (curr[i].vx - prev[i].vx) / delta_t;
            curr[i].ay = (curr[i].vy - prev[i].vy) / delta_t;
            curr[i].az = (curr[i].vz - prev[i].vz) / delta_t;
        }
    }
}