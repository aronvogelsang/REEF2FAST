#include "elevation_elev2d.hpp"
#include "common.hpp"
#include "../external/nanoflann.hpp"
#include <map>
#include <cmath>
#include <vector>
#include <algorithm>

// Internal KDTree structure for 1D interpolation
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

void compute_surface_elevation_from_elev_2d_single_timestep(
    std::vector<WavefieldEntry>& target,
    const std::vector<WavefieldEntry>& raw)
{
    std::map<double, double> max_elev_map;

    for (const auto& e : raw) {
        double x = round_to(e.x);
        max_elev_map[x] = std::max(max_elev_map[x], e.elevation);
    }

    XCloud cloud;
    for (const auto& [x, val] : max_elev_map) {
        cloud.pts.push_back(x);
        cloud.values.push_back(val);
    }

    KDTree1D tree(1, cloud, nanoflann::KDTreeSingleIndexAdaptorParams(10));
    tree.buildIndex();

    for (auto& e : target) {
        double query[1] = {e.x};

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

        e.elevation = weighted_val / sum_w;
    }
}