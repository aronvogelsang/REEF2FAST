// cloud.cpp
#include "cloud.hpp"
#include "common.hpp"
#include "structs.hpp"
#include "report_diagnostics.hpp"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <omp.h>

#include "../external/nanoflann.hpp"

// Internal struct for KDTree
struct PointCloud {
    struct Point { double x, y, z; };
    std::vector<Point> pts;

    inline size_t kdtree_get_point_count() const { return pts.size(); }
    inline double kdtree_get_pt(const size_t idx, const size_t dim) const {
        if (dim == 0) return pts[idx].x;
        if (dim == 1) return pts[idx].y;
        return pts[idx].z;
    }
    template <class BBOX> bool kdtree_get_bbox(BBOX&) const { return false; }
};

// Alias for nanoflann KDTree
using KDTree = nanoflann::KDTreeSingleIndexAdaptor<
    nanoflann::L2_Simple_Adaptor<double, PointCloud>,
    PointCloud,
    3
>;

// Generates OpenFAST SeaState target grid based on control.txt
void generate_seastate_grid_targets(const std::string& control_file,
                                    std::vector<std::array<double, 3>>& targets) {
    double X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX;
    int NX, NY, NZ;
    if (!read_control_file(control_file, X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX, NX, NY, NZ)) {
        std::cerr << "Failed to read control file: " << control_file << std::endl;
        return;
    }

    int sizeX = NX - 1;
    int sizeY = NY - 1;
    int sizeZ = NZ;

    auto x_grid = generate_linear_grid(X_MIN, X_MAX, sizeX);
    auto y_grid = generate_linear_grid(Y_MIN, Y_MAX, sizeY);

    double Z_Depth = Z_MAX - Z_MIN;
    double dthetaZ = M_PI / (2.0 * (sizeZ - 1));
    std::vector<double> z_grid;
    for (int i = 0; i < sizeZ; ++i) {
        double z = (cos(i * dthetaZ) - 1.0) * Z_Depth;
        z_grid.push_back(z);
    }

    for (const auto& x : x_grid) {
        for (const auto& y : y_grid) {
            for (const auto& z : z_grid) {
                targets.push_back({x, y, z});
            }
        }
    }

}

// Interpolates a scalar field to the given grid using inverse-distance weighting
std::vector<double> interpolate_to_grid(const Wavefield& wf,
                                        const std::vector<std::array<double, 3>>& target_pts,
                                        const std::string& field,
                                        int k) {
    PointCloud cloud;
    std::vector<double> values;

    for (const auto& e : wf) {
        cloud.pts.push_back({e.x, e.y, e.z});
        if (field == "vx") values.push_back(e.vx);
        else if (field == "vy") values.push_back(e.vy);
        else if (field == "vz") values.push_back(e.vz);
        else if (field == "pressure") values.push_back(e.pressure);
    }

    KDTree tree(3, cloud, nanoflann::KDTreeSingleIndexAdaptorParams(10));
    tree.buildIndex();

    std::vector<double> result(target_pts.size());

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < static_cast<int>(target_pts.size()); ++i) {
        const auto& pt = target_pts[i];

        std::vector<size_t> indices(k);
        std::vector<double> dists(k);
        nanoflann::KNNResultSet<double> resultSet(k);
        resultSet.init(indices.data(), dists.data());
        tree.findNeighbors(resultSet, pt.data(), nanoflann::SearchParameters(10));

        double sum_weights = 0.0, weighted_val = 0.0;
        for (size_t j = 0; j < resultSet.size(); ++j) {
            double dist = std::sqrt(dists[j]) + 1e-6;
            double w = 1.0 / dist;
            sum_weights += w;
            weighted_val += w * values[indices[j]];
        }

        result[i] = weighted_val / sum_weights;
    }

    return result;
}
