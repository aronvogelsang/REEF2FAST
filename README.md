# REEF2FAST

**Version:** 2.2  
**Developer:** Aron Vogelsang  
**License:** GNU GPL v3.0  
**Repository:** https://github.com/aronvogelsang/REEF2FAST  

## Overview

REEF2FAST is a pre-processor unit that transforms REEF3D wavefield outputs into OpenFAST-compatible input via SeaState.  
It supports both 2D and 3D wavefields and uses a streaming-based processing pipeline to minimize memory usage, enabling fast and efficient handling of large datasets.

---

## Installation

### Prerequisites

- C++17 compatible compiler (e.g., `g++`, `clang++`)
- OpenMP support (for parallel processing)
- CMake (version ≥ 3.10)
- Make
- Git

### Build Instructions

```bash
git clone https://github.com/aronvogelsang/REEF2FAST.git
cd REEF2FAST
mkdir build && cd build
cmake ..
make
```

The executable `reef2fast` will be created in the `build/` directory.  
**Run the program from there.**

---

## Usage

### Preparation

Ensure the following input files are placed in the `data/` directory:

1. `control.txt` – DiveMESH grid & domain configuration  
2. `ctrl.txt` – REEF3D simulation settings (Hs, Tp, duration, timestep)  
3. `XXX.csv` – Extracted REEF3D wavefield file (exported from ParaView)

> **CSV Format Note:**  
> The REEF3D CSV file must contain a header in the following format:  
> `timestep,u,v,w,eta,pressure,x,y,z`  
> Make sure to Tick in ParaView: "Write Time Steps" and  "Add Time Step"

### Run

```bash
./reef2fast
```

The program will:

1. Parse control parameters
2. Interpolate REEF3D wavefield data onto the OpenFAST SeaState grid
3. Compute velocity, acceleration, and surface elevation
4. Export OpenFAST-compatible `.Vi`, `.Ai`, `.DynP`, `.Elev` files
5. Optionally write a CSV file (`interpolated_wavefield.csv`) for diagnostics

---

## Output Files

All output files will be placed in the `output/` directory:

- `REEF2FAST.Vxi`, `.Vyi`, `.Vzi` – velocity components  
- `REEF2FAST.Axi`, `.Ayi`, `.Azi` – acceleration components  
- `REEF2FAST.DynP` – dynamic pressure  
- `REEF2FAST.Elev` – surface elevation  
- `interpolated_wavefield.csv` – optional timestep-wise diagnostics  
- `REEF2FAST.dat` – SeaState grid configuration file for OpenFAST

---

## Compatibility

- **OpenFAST:** v4.0.2 (tested)  
- **REEF3D:** v25.02 (tested)  
- **Supported Solver:** REEF3D::NHFLOW  
  *(Other solvers may work if the CSV header matches the required format: `"TimeStep","velocity:0","velocity:1","velocity:2","pressure","elevation","Points:0","Points:1","Points:2"`)*

---
