#include "elevation_geo.hpp"
#include "common.hpp"
#include "../external/nanoflann.hpp"
#include <map>
#include <cmath>
#include <vector>
#include <array>
#include <algorithm>

// Simple alias to represent (x, y) key
using XY = std::pair<double, double>;

// KDTree structure for interpolating z-surface over (x, y)
struct XYCloud {
    std::vector<XY> pts;
    std::vector<double> values;

    inline size_t kdtree_get_point_count() const { return pts.size(); }
    inline double kdtree_get_pt(const size_t idx, const size_t dim) const {
        return dim == 0 ? pts[idx].first : pts[idx].second;
    }
    template <class BBOX> bool kdtree_get_bbox(BBOX&) const { return false; }
};

using KDTree2D = nanoflann::KDTreeSingleIndexAdaptor<
    nanoflann::L2_Simple_Adaptor<double, XYCloud>,
    XYCloud,
    2
>;

void compute_surface_elevation_geo_single_timestep(std::vector<WavefieldEntry>& target,
                                                   const std::vector<WavefieldEntry>& raw) {
    std::map<XY, double> max_z_map;

    // Find max z for each (x, y)
    for (const auto& entry : raw) {
        XY key = {round_to(entry.x), round_to(entry.y)};
        if (max_z_map.find(key) == max_z_map.end()) {
            max_z_map[key] = entry.z;
        } else {
            max_z_map[key] = std::max(max_z_map[key], entry.z);
        }
    }

    // Build KDTree for interpolation
    XYCloud cloud;
    for (const auto& [xy, zval] : max_z_map) {
        cloud.pts.push_back(xy);
        cloud.values.push_back(zval);
    }

    KDTree2D tree(2, cloud, nanoflann::KDTreeSingleIndexAdaptorParams(10));
    tree.buildIndex();

    // Interpolate onto SeaState target points
    for (auto& pt : target) {
        double query[2] = {pt.x, pt.y};

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

        pt.elevation = weighted_val / sum_w;
    }
}