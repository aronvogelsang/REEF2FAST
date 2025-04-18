#pragma once

#include "structs.hpp"
#include <string>
#include <functional>

/**
 * Writes a single wave kinematics component (e.g. vx, ax, pressure)
 * to a SeaState-formatted .XXX file.
 *
 * @param wf              Interpolated wavefield at current timestep
 * @param filename        Output filename (e.g. "REEF2FAST.Vxi")
 * @param component_name  Internal name of the field ("vx", "pressure", etc.)
 * @param description     Human-readable description for the header
 * @param accessor        Lambda to extract the relevant value from a WavefieldEntry
 * @param wave_dt         Time step in seconds
 * @param timestep        Current timestep index
 * @param append          If true, appends to existing file instead of overwriting
 */
void write_wave_component(const Wavefield& wf,
                          const std::string& filename,
                          const std::string& component_name,
                          const std::string& description,
                          std::function<double(const WavefieldEntry&)> accessor,
                          double wave_dt,
                          int timestep,
                          bool append = false);

/**
 * Writes all standard fields (vx, vy, vz, ax, ay, az, pressure)
 * for the given timestep in SeaState format.
 */
void generate_all_wavefiles(const Wavefield& wf,
                            double wave_dt,
                            int timestep,
                            bool append = false);