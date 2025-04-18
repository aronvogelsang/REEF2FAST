#include "streamingpipeline.hpp"
#include "common.hpp"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {

    std::cout << "**************************************************************************\n";
    std::cout << "*  REEF2FAST v2.1 - OpenFAST Input Generator for REEF3D Wavefields       *\n";
    std::cout << "*                                                                        *\n";    
    std::cout << "*  GitHub: https://github.com/aronvogelsang/REEF2FAST                    *\n";
    std::cout << "*  Developer: Aron Vogelsang                                             *\n";
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

    // Ensure output folder exists
    if (!fs::exists("../output")) {
        fs::create_directory("../output");
    }

    // Detect if case is 2D based on Y-dimension in control.txt
    bool is2D = is_2D_case("../data/control.txt");

    // Ask user for elevation method
    std::string elevation_mode;
    std::cout << "Surface elevation method ('z' = geometric, 'e' = hydrodynamic): ";
    std::cin >> elevation_mode;
    if (elevation_mode != "z" && elevation_mode != "e") {
        std::cerr << "Invalid input. Use 'z' or 'e'.\n";
        return 1;
    }

    // Ask user whether to write CSV export
    bool write_csv = false;
    std::string csv_answer;
    std::cout << "Write CSV output (interpolated_wavefield.csv)? (y/n): ";
    std::cin >> csv_answer;
    if (!csv_answer.empty() && (csv_answer[0] == 'y' || csv_answer[0] == 'Y')) {
        write_csv = true;
    }

    // If 2D, ask for manual y-domain and NY input
    double y_total = 0.0;
    int ny_usr = 0;
    if (is2D) {
        std::cout << "\nDetected 2D wavefield. Manual Y-domain input required.\n";
        std::cout << "Enter Y domain width (in meters): ";
        std::cin >> y_total;

        std::cout << "Enter number of grid points NY (even number >= 4): ";
        std::cin >> ny_usr;
        while (ny_usr < 4 || ny_usr % 2 != 0) {
            std::cerr << "NY must be even and >= 4. Try again: ";
            std::cin >> ny_usr;
        }
    } else {
        std::cout << "\nDetected 3D wavefield.\n";
    }

    // Automatically detect wavefield CSV file in ../data/
    std::string wavefield_file;
    try {
        wavefield_file = find_wavefield_file("../data/");
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    // Launch the streaming pipeline
    try {
        StreamingPipeline pipeline(
            wavefield_file,
            "../data/control.txt",
            "../data/ctrl.txt",
            is2D,
            elevation_mode,
            write_csv,
            y_total,
            ny_usr
        );

        pipeline.run();
        std::cout << "\nREEF2FAST pipeline finished successfully.\n";

    } catch (const std::exception& e) {
        std::cerr << "\nPipeline failed: " << e.what() << "\n";
        return 1;
    }

    return 0;
}