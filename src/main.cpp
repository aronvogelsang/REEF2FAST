#include "cloud.hpp"
#include "acc.hpp"
#include "elevation_geo.hpp"
#include "elevation_elev.hpp"
#include "genSeaState.hpp"
#include "genAyi.hpp"
#include "export.hpp"
#include "export_elevation.hpp"
#include "common.hpp"
#include "write_out.hpp"
#include "cloud2d.hpp"
#include "elevation_elev2d.hpp"
#include "elevation_geo2d.hpp"
#include "inflate2d.hpp"
#include "acc2d.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {

    std::cout << "**************************************************************************\n";
    std::cout << "*  REEF2FAST v1.0 - OpenFAST Input Generator for REEF3D Wavefields       *\n";
    std::cout << "*                                                                        *\n";    
    std::cout << "*  GitHub: https://github.com/REEF2FAST                                  *\n";
    std::cout << "*  Author: Aron Vogelsang                                                *\n";
    std::cout << "*  License: GNU General Public License v3.0 (GPL-3.0)                    *\n";
    std::cout << "*                                                                        *\n";
    std::cout << "*  This program generates OpenFAST input files based on the REEF3D       *\n";
    std::cout << "*  simulation data. It reads control parameters from 'control.txt' and   *\n";
    std::cout << "*  'ctrl.txt', interpolates wavefield data, and formats it according to  *\n";
    std::cout << "*  OpenFAST standards.                                                   *\n";
    std::cout << "*                                                                        *\n";
    std::cout << "*  Ensure that the required input files are present:                     *\n";
    std::cout << "*  - 'control.txt' (grid & domain settings)                              *\n";
    std::cout << "*  - 'ctrl.txt' (wave parameters)                                        *\n";
    std::cout << "*  - 'XXX.csv' (REEF3D wavefield data)                                   *\n";
    std::cout << "*                                                                        *\n";
    std::cout << "*  Supported REEF3D solvers:                                             *\n";
    std::cout << "*  - REEF3D::NHFLOW (Non-Hydrostatic Flow Solver)                        *\n";
    std::cout << "*  - REEF3D::CFD (Computational Fluid Dynamics Solver)                   *\n";
    std::cout << "*                                                                        *\n";
    std::cout << "*  Compatibility:                                                        *\n";
    std::cout << "*  - OpenFAST v4.0.2                                                     *\n";
    std::cout << "*  - REEF3D v25.02                                                       *\n";
    std::cout << "*                                                                        *\n";
    std::cout << "*  WARNING:                                                              *\n";
    std::cout << "*  This program is designed and tested for the above versions. Using     *\n";
    std::cout << "*  other versions of OpenFAST or REEF3D may lead to unexpected behavior  *\n";
    std::cout << "*  or failures during execution.                                         *\n";
    std::cout << "*                                                                        *\n";
    std::cout << "*  License Terms:                                                        *\n";
    std::cout << "*  This program is free software: you can redistribute it and/or modify  *\n";
    std::cout << "*  it under the terms of the GNU General Public License as published by  *\n";
    std::cout << "*  the Free Software Foundation, either version 3 of the License, or     *\n";
    std::cout << "*  (at your option) any later version.                                   *\n";
    std::cout << "*                                                                        *\n";
    std::cout << "*  This program is distributed in the hope that it will be useful,       *\n";
    std::cout << "*  but WITHOUT ANY WARRANTY; without even the implied warranty of        *\n";
    std::cout << "*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *\n";
    std::cout << "*  GNU General Public License for more details.                          *\n";
    std::cout << "*                                                                        *\n";
    std::cout << "*  You should have received a copy of the GNU General Public License     *\n";
    std::cout << "*  along with this program. If not, see <https://www.gnu.org/licenses/>. *\n";
    std::cout << "**************************************************************************\n";
    std::cout << "\n";
    std::cout << "\n";
    std::cout << "\n";
    std::cout << "\n";

    std::string mode;
    std::string csv_reply;
    double Y_total;
    int NY_usr;
    InterpolationResult result;
    InterpolatedWavefield* interp_result = nullptr;
    WavefieldByTime* raw_wavefield = nullptr;
    double wave_dt = 0.0;
    std::string wavefield_path;

    // User chooses elevation mode
    std::cout << "\nWhich model for elevation computation?\n";
    std::cout << "Type 'z' for geometric (z_max) or 'e' for hydrodynamic (eta): ";
    std::cin >> mode;

    if (mode != "z" && mode != "e") {
        std::cerr << "Invalid input. Use 'z' or 'e'.\n";
        return 1;
    }

    // User chooses if he wants the full interpolated wavefield
    bool csv_enabled = false;
    std::cout << "\nOutput file with all interpolated fields (timestep,point,vel,pressure,eta,acc) required? (y/n): ";
    std::cin >> csv_reply;
    csv_enabled = (std::tolower(csv_reply[0]) == 'y');

    // Create output directory if it does not exist
    if (!fs::exists("../output")) {
        fs::create_directory("../output");
    }

    // Detect 2D case
    bool is2D = is_2D_case("../data/control.txt");
    if (is2D) {
        std::cout << "\nDetected 2D wavefield. Please define Y domain manually.\n";
        std::cout << "Enter domain width in Y-direction (in meters): ";
        std::cin >> Y_total;
        std::cout << "Enter number of grid points in Y-direction (NY >= 4 and NY%2 = 0): ";
        std::cin >> NY_usr;

        while (NY_usr < 4 || NY_usr % 2 != 0) {
            std::cerr << "Invalid input. NY must be NY >= 4 and NY%2 = 0.\n";
            std::cout << "Enter number of grid points in Y-direction (NY >= 4 and NY%2 = 0): ";
            std::cin >> NY_usr;
        }

    } else {
        std::cout << "Detected 3D wavefield. Running REEF2FAST in 3D mode.\n";
    }

    std::cout << "\nREEF2FAST WAVEFIELD PIPELINE STARTED\n";

    try {
        wavefield_path = find_wavefield_file("../data/");
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
        

       

       
    
        
       
    
    if (is2D) {
    
        // Step 1: Interpolation    
        std::cout << "\nStep 1: Interpolating 2D wavefield...\n";    
        result = interpolate_full_wavefield_2d(wavefield_path, "../data/control.txt");    
        interp_result = &result.interpolated;
        raw_wavefield = &result.raw_data;    
        std::cout << "Interpolation complete.\n";
    
        // Step 2: Surface elevation (z or elevation based)
        std::cout << "\nStep 2: Computing surface elevation (2D)...\n";
        if (mode == "z") {
             compute_surface_elevation_geo_2d(*interp_result, *raw_wavefield);
        } else {
            compute_surface_elevation_from_elev_2d(*interp_result, *raw_wavefield);
        }
        std::cout << "Elevation computed.\n";
    } else {    
    
        // Step 1: Interpolation
        std::cout << "\nStep 1: Interpolating 3D wavefield...\n";
        result = interpolate_full_wavefield(wavefield_path, "../data/control.txt");
        interp_result = &result.interpolated;
        raw_wavefield = &result.raw_data;
        std::cout << "Interpolation complete.\n";
    
        // Step 2: Surface elevation (z or elevation based)
        std::cout << "\nStep 2: Computing surface elevation (3D)...\n";
        if (mode == "z") {
            compute_surface_elevation(*interp_result, *raw_wavefield);
        } else {
            compute_surface_elevation_from_elev(*interp_result, *raw_wavefield);
        }
        std::cout << "Elevation computed.\n";
    }
    
    // Step 3: Free raw wavefield from memory
    WavefieldByTime().swap(*raw_wavefield);
    
    // Step 4: Compute acceleration (2D or 3D)
    std::cout << "\nStep 3: Computing acceleration...\n";
    if (!read_timestep_from_control("../data/ctrl.txt", wave_dt)) {
        std::cerr << "Failed to read timestep from control file.\n";
        return 1;
    }
    
    if (is2D) {
        computeAcceleration2D(*interp_result, wave_dt);
    } else {
        computeAcceleration(*interp_result, wave_dt);
    }
        std::cout << "Acceleration computed.\n";

    // 2D Wavefield inflation        
        if (is2D) {
            inflate_wavefield_y(*interp_result, Y_total, NY_usr);}

        // Step 4: Generate SeaState input
        std::cout << "\nStep 4: Generating SeaState input file...\n";
        double X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX;
        int NX, NY, NZ;
        double sim_time, wave_hs, wave_tp;
        if (!read_control_file("../data/control.txt", X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX, NX, NY, NZ)) return 1;
        if (!read_wave_parameters("../data/ctrl.txt", sim_time, wave_dt, wave_hs, wave_tp)) return 1;

        if (is2D) {
            Y_MIN = - Y_total;
            Y_MAX = Y_total;
            NY    = NY_usr;
        }

        generate_seastate(X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX, NX, NY, NZ, sim_time, wave_dt, wave_hs, wave_tp);
        std::cout << "REEF2FAST.dat written.\n";

        // Step 5: Export .XXX files
        std::cout << "\nStep 5: Exporting SeaState wave component files...\n";
        generate_all_wavefiles(*interp_result, wave_dt, sim_time);
        write_surface_elevation(*interp_result, "REEF2FAST.Elev", wave_dt, sim_time);
        std::cout << "All SeaState files written.\n";


        // Optional CSV Output
        if (csv_enabled) {
            std::cout << "\nStep 6: Writing combined CSV...\n";
            write_out_csv(*interp_result, "../output/interpolated_wavefield.csv");
            std::cout << "CSV written to interpolated_wavefield.csv\n";
        }

    std::cout << "\nProgram finished successfully.\n";        return 0;
   
}


