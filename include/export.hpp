// export.hpp
#pragma once

#include "cloud.hpp"
#include <string>
#include <functional>

/**
 * Writes a specific wave kinematics component (e.g. vx, ax, pressure) to a SeaState-formatted .XXX file.
 *
 * @param wf              Interpolated wavefield
 * @param filename        Output filename (e.g. "REEF2FAST.Vxi")
 * @param component_name  Name of the component ("vx", "pressure", etc.)
 * @param description     Human-readable description written in header
 * @param accessor        Lambda or function to access the desired field from a WavefieldEntry
 * @param wave_dt         Timestep in seconds (from ctrl.txt)
 * @param wave_tmax       Total wave time (from ctrl.txt)
 */
void write_wave_component(const InterpolatedWavefield& wf,
                          const std::string& filename,
                          const std::string& component_name,
                          const std::string& description,
                          std::function<double(const WavefieldEntry&)> accessor,
                          double wave_dt, double wave_tmax);

/**
 * Calls write_wave_component() for all standard wave fields (vx, vy, vz, ax, ay, az, pressure).
 */
void generate_all_wavefiles(const InterpolatedWavefield& wf, double wave_dt, double wave_tmax);