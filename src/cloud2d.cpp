#include "cloud.hpp"
#include "common.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <omp.h>
#include "../external/nanoflann.hpp"

struct PointCloud2D {
    struct Point { double x, z; };
    std::vector<Point> pts;

    inline size_t kdtree_get_point_count() const { return pts.size(); }
    inline double kdtree_get_pt(const size_t idx, const size_t dim) const {
        return (dim == 0) ? pts[idx].x : pts[idx].z;
    }
    template <class BBOX> bool kdtree_get_bbox(BBOX&) const { return false; }
};

using KDTree2D = nanoflann::KDTreeSingleIndexAdaptor<
    nanoflann::L2_Simple_Adaptor<double, PointCloud2D>,
    PointCloud2D,
    2
>;

// Only keep 1 y-slice
WavefieldByTime read_wavefield_csv_2d(const std::string& filename, double z_max) {
    WavefieldByTime data;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open " << filename << std::endl;
        return data;
    }

    std::string line;
    std::getline(file, line); // skip header

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

        if (std::isnan(y_ref)) {
            y_ref = entry.y; // Use first y-value as reference
        }

        if (std::abs(entry.y - y_ref) > 1e-6) continue; // Skip non-slice values

        entry.z = round_to(entry.z - z_max);
        entry.elevation = round_to(entry.elevation - z_max);

        data[timestep].push_back(entry);
    }

    file.close();
    size_t total = 0;
    for (const auto& [ts, vec] : data) total += vec.size();
    std::cout << "Wavefield (2D): " << total << " points loaded across " << data.size() << " timesteps (y ≈ " << y_ref << ").\n";
    return data;
}

void generate_seastate_grid_targets_2d(const std::string& control_file,
                                       std::vector<std::array<double, 3>>& targets) {
    double X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX;
    int NX, NY, NZ;
    if (!read_control_file(control_file, X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX, NX, NY, NZ)) {
        std::cerr << "Failed to read control file: " << control_file << std::endl;
        return;
    }

    int sizeX = NX - 1;
    int sizeZ = NZ;

    auto x_grid = generate_linear_grid(X_MIN, X_MAX, sizeX);

    double Z_Depth = Z_MAX - Z_MIN;
    double dthetaZ = M_PI / (2.0 * (sizeZ - 1));
    std::vector<double> z_grid;
    for (int i = 0; i < sizeZ; ++i) {
        double z = (cos(i * dthetaZ) - 1.0) * Z_Depth;
        z_grid.push_back(z);
    }

    for (const auto& x : x_grid) {
        for (const auto& z : z_grid) {
            targets.push_back({x, 0.0, z}); // y = 0.0 placeholder
        }
    }

    std::cout << "2D base grid created (x–z plane only):\n";
    std::cout << "  X: " << sizeX << " points from " << X_MIN << " to " << X_MAX << "\n";
    std::cout << "  Z: " << sizeZ << " points from " << Z_MIN << " to " << Z_MAX << "\n";
    std::cout << "  Total 2D target points: " << targets.size() << " .\n";
}

std::vector<double> interpolate_to_grid_2d(const Wavefield& wf,
                                           const std::vector<std::array<double, 3>>& target_pts,
                                           const std::string& field,
                                           int k = 4) {

    PointCloud2D cloud;
    std::vector<double> values;

    for (const auto& e : wf) {
        cloud.pts.push_back({e.x, e.z});
        if (field == "vx") values.push_back(e.vx);
        else if (field == "vz") values.push_back(e.vz);
        else if (field == "pressure") values.push_back(e.pressure);
    }

    KDTree2D tree(2, cloud, nanoflann::KDTreeSingleIndexAdaptorParams(10));
    tree.buildIndex();

    std::vector<double> result(target_pts.size());

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < static_cast<int>(target_pts.size()); ++i) {
        const auto& pt = target_pts[i];
        double query_pt[2] = { pt[0], pt[2] };

        std::vector<size_t> indices(k);
        std::vector<double> dists(k);
        nanoflann::KNNResultSet<double> resultSet(k);
        resultSet.init(indices.data(), dists.data());
        tree.findNeighbors(resultSet, query_pt, nanoflann::SearchParameters(10));

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

InterpolationResult interpolate_full_wavefield_2d(const std::string& wavefield_file,
    const std::string& control_file) {
    double X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX;
    int NX, NY, NZ;
    if (!read_control_file(control_file, X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX, NX, NY, NZ)) {
    throw std::runtime_error("Failed to read control file");
    }

    double z_max = Z_MAX;
    WavefieldByTime wavefield_by_time = read_wavefield_csv_2d(wavefield_file, z_max);

    std::vector<std::array<double, 3>> target_points;
    generate_seastate_grid_targets_2d(control_file, target_points);

    InterpolatedWavefield result;
    result.grid_points = target_points;
    result.timesteps.resize(wavefield_by_time.size());

    int ts_index = 0;

    for (const auto& [timestep, wf] : wavefield_by_time) {
        std::cout << "\nTimestep: " << timestep << std::endl;

        auto vx_interp = interpolate_to_grid_2d(wf, target_points, "vx");
        auto vz_interp = interpolate_to_grid_2d(wf, target_points, "vz");
        auto p_interp  = interpolate_to_grid_2d(wf, target_points, "pressure");

        std::vector<WavefieldEntry> timestep_data;
    for (size_t i = 0; i < target_points.size(); ++i) {
        const auto& pt = target_points[i];
        timestep_data.push_back(WavefieldEntry{
        pt[0], pt[1], pt[2],
        vx_interp[i], 0.0, vz_interp[i], // vy is set to 0.0 for 2D wavefields
        p_interp[i], NAN,
        NAN, NAN, NAN
    });
    }
    // Optional: max diagnostics
    double max_vx = -1e10, max_vz = -1e10, max_p = -1e10;
    for (const auto& e : timestep_data) {
        max_vx = std::max(max_vx, std::abs(e.vx));
        max_vz = std::max(max_vz, std::abs(e.vz));
        max_p  = std::max(max_p,  std::abs(e.pressure));
        }

    std::cout << "  Max |u|: " << max_vx
    << ", |w|: " << max_vz
    << ", |p|: "  << max_p << std::endl;

    result.timesteps[ts_index++] = std::move(timestep_data);
    }


    return InterpolationResult{ result, wavefield_by_time };
}