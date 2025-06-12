#include "wavefield_streaming.hpp"
#include "common.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <set>
#include <cmath>

// Stream wavefield CSV in 3D: passes (prev, curr, next) to the callback as they become available
void stream_wavefield_with_context(
    const std::string& filename,
    double z_max,
    std::function<void(int,
                       const std::vector<WavefieldEntry>&,
                       const std::vector<WavefieldEntry>&,
                       const std::vector<WavefieldEntry>&)> callback)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open wavefield CSV: " + filename);
    }

    std::string line;
    std::getline(file, line); // Skip CSV header

    std::map<int, std::vector<WavefieldEntry>> buffer;
    std::set<int> called_timesteps;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        int timestep;
        char comma;
        WavefieldEntry entry;

        ss >> timestep >> comma
           >> entry.vx >> comma
           >> entry.vy >> comma
           >> entry.vz >> comma
           >> entry.pressure >> comma
           >> entry.elevation >> comma
           >> entry.x >> comma
           >> entry.y >> comma
           >> entry.z;
           
        // Convert from REEF3D vertical system to OpenFAST convention (z=0 at SWL, negative downward)
        entry.z = round_to(entry.z - z_max);
        entry.elevation = round_to(entry.elevation - z_max);

        buffer[timestep].push_back(entry);

        // Process timesteps in order: always t0 first (including t=0)
        while (buffer.size() >= 3) {
            auto it = buffer.begin();
            int t0 = it->first; auto& wf0 = it->second; ++it;
            int t1 = it->first; auto& wf1 = it->second; ++it;
            int t2 = it->first; auto& wf2 = it->second;

            if (t0 == 0 && called_timesteps.count(0) == 0) {
                callback(0, wf0, wf0, wf1);  // Special handling for t=0
                called_timesteps.insert(0);
            }

            if (called_timesteps.count(t1) == 0) {
                callback(t1, wf0, wf1, wf2);
                called_timesteps.insert(t1);
            }

            buffer.erase(t0);  // Slide window
        }
    }

    // Final fallback: process the last two remaining timesteps
    if (buffer.size() >= 2) {
        auto it = buffer.begin();
        int t0 = it->first; auto& wf0 = it->second; ++it;
        int t1 = it->first; auto& wf1 = it->second;

        if (called_timesteps.count(0) == 0 && t0 == 0) {
            callback(0, wf0, wf0, wf1);
            called_timesteps.insert(0);
        }

        std::vector<WavefieldEntry> dummy_next;
        if (called_timesteps.count(t1) == 0) {
            callback(t1, wf0, wf1, dummy_next);
            called_timesteps.insert(t1);
        }
    }

    file.close();
}

// Stream wavefield CSV in 2D: selects a reference y-slice and processes timesteps streamingly
void stream_wavefield_with_context_2d(
    const std::string& filename,
    double z_max,
    std::function<void(int,
                       const std::vector<WavefieldEntry>&,
                       const std::vector<WavefieldEntry>&,
                       const std::vector<WavefieldEntry>&)> callback)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open wavefield CSV: " + filename);
    }

    std::string line;
    std::getline(file, line); // Skip CSV header

    std::map<int, std::vector<WavefieldEntry>> buffer;
    std::set<int> called_timesteps;
    double y_ref = NAN;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        int timestep;
        char comma;
        WavefieldEntry entry;

        ss >> timestep >> comma
           >> entry.vx >> comma
           >> entry.vy >> comma
           >> entry.vz >> comma
           >> entry.pressure >> comma
           >> entry.elevation >> comma
           >> entry.x >> comma
           >> entry.y >> comma
           >> entry.z;

        // Capture the first encountered y-coordinate as the reference slice
        if (std::isnan(y_ref)) {
            y_ref = entry.y;
        }

        // Skip all points not from the selected y-slice
        if (std::abs(entry.y - y_ref) > 1e-6) continue;

        // Collapse y to 0.0 for clean 2D wavefield alignment
        entry.y = 0.0;

        entry.z = round_to(entry.z - z_max);
        entry.elevation = round_to(entry.elevation - z_max);

        buffer[timestep].push_back(entry);

        // Process timesteps in order: always t0 first (including t=0)
        while (buffer.size() >= 3) {
            auto it = buffer.begin();
            int t0 = it->first; auto& wf0 = it->second; ++it;
            int t1 = it->first; auto& wf1 = it->second; ++it;
            int t2 = it->first; auto& wf2 = it->second;

            if (t0 == 0 && called_timesteps.count(0) == 0) {
                callback(0, wf0, wf0, wf1);  // Special handling for t=0
                called_timesteps.insert(0);
            }

            if (called_timesteps.count(t1) == 0) {
                callback(t1, wf0, wf1, wf2);
                called_timesteps.insert(t1);
            }

            buffer.erase(t0);  // Slide window
        }
    }

    // Final fallback: process the last two remaining timesteps
    if (buffer.size() >= 2) {
        auto it = buffer.begin();
        int t0 = it->first; auto& wf0 = it->second; ++it;
        int t1 = it->first; auto& wf1 = it->second;

        if (called_timesteps.count(0) == 0 && t0 == 0) {
            callback(0, wf0, wf0, wf1);
            called_timesteps.insert(0);
        }

        std::vector<WavefieldEntry> dummy_next;
        if (called_timesteps.count(t1) == 0) {
            callback(t1, wf0, wf1, dummy_next);
            called_timesteps.insert(t1);
        }
    }

    file.close();
}