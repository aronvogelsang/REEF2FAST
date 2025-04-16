#ifndef GENSEASTATE_HPP
#define GENSEASTATE_HPP

#include <string>

bool read_wave_parameters(const std::string& filename, double& sim_time, double& wave_dt, 
                          double& wave_hs, double& wave_tp);

void generate_seastate(double X_MIN, double X_MAX, double Y_MIN, double Y_MAX, double Z_MIN, double Z_MAX, 
                       int NX, int NY, int NZ, double sim_time, double wave_dt, double wave_hs, double wave_tp);

#endif // GENSEASTATE_HPP