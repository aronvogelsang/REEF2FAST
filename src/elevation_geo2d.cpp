#include "elevation_geo2d.hpp"
#include "common.hpp"
#include "../external/nanoflann.hpp"
#include <map>
#include <cmath>
#include <vector>
#include <algorithm>

// Internal KDTree structure for x-only interpolation
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

void compute_surface_elevation_geo_2d_single_timestep(std::vector<WavefieldEntry>& target,
                                                      const std::vector<WavefieldEntry>& raw) {
    std::map<double, std::vector<double>> x_to_zvals;

    // Group z-values for each x-coordinate (y is constant in 2D case)
    for (const auto& e : raw) {
        double x = round_to(e.x);
        x_to_zvals[x].push_back(e.z);
    }

    // Compute maximum z per vertical column
    XCloud cloud;
    cloud.pts.reserve(x_to_zvals.size());
    cloud.values.reserve(x_to_zvals.size());

    for (const auto& [x, vec] : x_to_zvals) {
        if (!vec.empty()) {
            double max_z = *std::max_element(vec.begin(), vec.end());
            cloud.pts.push_back(x);
            cloud.values.push_back(max_z);
        }
    }

    // Build KDTree for interpolation
    KDTree1D tree(1, cloud, nanoflann::KDTreeSingleIndexAdaptorParams(10));
    tree.buildIndex();

    // Interpolate to SeaState grid
    for (auto& pt : target) {
        double query[1] = {pt.x};

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
