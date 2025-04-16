/*#ifndef GENAYI_HPP
#define GENAYI_HPP

#include <vector>
#include <string>
#include "common.hpp"

std::vector<DataPoint> read_wavefield_csv(const std::string& filename);
bool read_wave_params(const std::string& filename, double& wave_dt, double& wave_tmax);
void write_seastate_ayi(const std::string& filename, const std::vector<DataPoint>& data, double wave_dt, double wave_tmax);
void generate_seastate_ayi();

#endif // GENAYI_HPP*/