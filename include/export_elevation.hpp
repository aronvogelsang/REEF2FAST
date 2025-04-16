#pragma once

#include "cloud.hpp"
#include <string>
#include <functional>

/**
 * Writes a specific wave kinematics component (e.g., vx, ax, pressure) to a SeaState-formatted .XXX file.
 *
 * @param wf              Interpolated wavefield
 * @param filename        Output filename (e.g., "REEF2FAST.Vxi")
 * @param component_name  Name of the component ("vx", "pressure", etc.)
 * @param description     Human-readable description for the file header
 * @param accessor        Lambda or function to access the desired field from a WavefieldEntry
 * @param wave_dt         Timestep size in seconds (from ctrl.txt)
 * @param wave_tmax       Total wave simulation time (from ctrl.txt)
 */
void write_wave_component(const InterpolatedWavefield& wf,
                          const std::string& filename,
                          const std::string& component_name,
                          const std::string& description,
                          std::function<double(const WavefieldEntry&)> accessor,
                          double wave_dt, double wave_tmax);

/**
 * Writes all standard wave components (.Vxi, .Axi, .DynP, etc.) to SeaState-formatted files.
 *
 * @param wf         Interpolated wavefield
 * @param wave_dt    Timestep size in seconds
 * @param wave_tmax  Total simulation time
 */
void generate_all_wavefiles(const InterpolatedWavefield& wf,
                            double wave_dt, double wave_tmax);

/**
 * Writes surface elevation data at z ≈ 0 (SWL) to a SeaState-compatible .Elev file.
 * Elevation values are sorted by y and x, and include full header metadata.
 *
 * @param wf         Interpolated wavefield (must contain elevation data)
 * @param filename   Output file name (e.g., "REEF2FAST.Elev")
 * @param wave_dt    Timestep size in seconds
 * @param wave_tmax  Total simulation time
 */
void write_surface_elevation(const InterpolatedWavefield& wf,
                             const std::string& filename,
                             double wave_dt, double wave_tmax);