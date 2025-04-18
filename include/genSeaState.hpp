// genSeaState.hpp
#pragma once

#include <string>

/**
 * Reads key wave parameters from the REEF3D ctrl.txt file.
 * Extracts: total simulation time, timestep, significant wave height and peak period.
 *
 * @param filename     Path to the REEF3D ctrl.txt file
 * @param sim_time     [out] Total simulation time
 * @param wave_dt      [out] Wave time step
 * @param wave_hs      [out] Significant wave height
 * @param wave_tp      [out] Peak spectral period
 * @return true if all parameters were successfully extracted
 */
bool read_wave_parameters(const std::string& filename,
                          double& sim_time,
                          double& wave_dt,
                          double& wave_hs,
                          double& wave_tp);

/**
 * Generates the main SeaState input file (REEF2FAST.dat) for OpenFAST.
 * This file defines the wave domain geometry, simulation time and links to external wave data.
 *
 * @param X_MIN, X_MAX   Domain limits in x-direction
 * @param Y_MIN, Y_MAX   Domain limits in y-direction
 * @param Z_MIN, Z_MAX   Domain limits in z-direction
 * @param NX, NY, NZ     Grid points in x/y/z directions
 * @param sim_time       Total wave simulation time
 * @param wave_dt        Wave time step
 * @param wave_hs        Significant wave height
 * @param wave_tp        Peak spectral period
 */
void generate_seastate(double X_MIN, double X_MAX,
                       double Y_MIN, double Y_MAX,
                       double Z_MIN, double Z_MAX,
                       int NX, int NY, int NZ,
                       double sim_time, double wave_dt,
                       double wave_hs, double wave_tp);