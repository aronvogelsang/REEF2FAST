#include "cloud.hpp"
#include "common.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <cmath>
#include <vector>
#include <array>
#include <tuple>
#include <algorithm>
#include "../external/nanoflann.hpp"

// Simple alias to represent 1D x-position
using X = double;

// Internal KDTree structure for 1D interpolation (x-only)
struct XCloud {
    std::vector<double> pts;
    std::vector<double> values;

    inline size_t kdtree_get_point_count() const { return pts.size(); }
    inline double kdtree_get_pt(const size_t idx, const size_t) const { return pts[idx]; }
    template <class BBOX> bool kdtree_get_bbox(BBOX&) const { return false; }
};

using KDTree1D = nanoflann::KDTreeSingleIndexAdaptor<
    nanoflann::L2_Simple_Adaptor<double, XCloud>,
    XCloud,
    1
>;

void compute_surface_elevation_from_elev_2d(InterpolatedWavefield& wf,
                                            const WavefieldByTime& original_data) {
    if (wf.timesteps.size() != original_data.size()) {
        std::cerr << "Mismatch in timestep sizes!\n";
        return;
    }

    size_t timestep_idx = 0;
    for (const auto& [timestep, raw_wavefield] : original_data) {
        std::map<X, double> max_elev_map;

        // 1. Max elevation at each x (y is constant anyway)
        for (const auto& entry : raw_wavefield) {
            double x = round_to(entry.x);
            if (max_elev_map.find(x) == max_elev_map.end()) {
                max_elev_map[x] = entry.elevation;
            } else {
                max_elev_map[x] = std::max(max_elev_map[x], entry.elevation);
            }
        }

        // 2. Build 1D KDTree
        XCloud cloud;
        for (const auto& [x, val] : max_elev_map) {
            cloud.pts.push_back(x);
            cloud.values.push_back(val);
        }

        KDTree1D tree(1, cloud, nanoflann::KDTreeSingleIndexAdaptorParams(10));
        tree.buildIndex();

        // 3. Interpolate onto wf.grid_points (using x only)
        auto& timestep_data = wf.timesteps[timestep_idx];
        for (size_t i = 0; i < wf.grid_points.size(); ++i) {
            const auto& pt = wf.grid_points[i];
            double query[1] = {pt[0]};  // only x

            std::vector<size_t> indices(4);
            std::vector<double> dists(4);
            nanoflann::KNNResultSet<double> resultSet(4);
            resultSet.init(indices.data(), dists.data());
            tree.findNeighbors(resultSet, query, nanoflann::SearchParameters(10));

            double sum_w = 0.0, weighted_val = 0.0;
            for (size_t j = 0; j < resultSet.size(); ++j) {
                double dist = std::sqrt(dists[j]) + 1e-6;
                double w = 1.0 / dist;
                weighted_val += w * cloud.values[indices[j]];
                sum_w += w;
            }

            double elev_interp = weighted_val / sum_w;
            timestep_data[i].elevation = elev_interp;
        }

        ++timestep_idx;
    }

    std::cout << "Hydrodynamic Elevation interpolation (2D) complete across " << timestep_idx << " timesteps.\n";
}