cmake_minimum_required(VERSION 3.10)
project(REEF2FAST)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optional: Allow user to override compiler
if(APPLE)
    message(STATUS "Using AppleClang on macOS")

    set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
    set(OpenMP_CXX_LIB_NAMES "omp")
    set(OpenMP_omp_LIBRARY "/opt/homebrew/opt/libomp/lib/libomp.dylib")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -I/opt/homebrew/opt/libomp/include")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/opt/homebrew/opt/libomp/lib -lomp")
endif()

# Try to find OpenMP
find_package(OpenMP COMPONENTS CXX)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found – compilation will be multi-threaded.")
else()
    message(WARNING "OpenMP not found – compilation will be single-threaded.")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/external)

# Collect all source and header files
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADER_FILES ${CMAKE_SOURCE_DIR}/include/*.hpp)

# Define the executable
add_executable(REEF2FAST ${SRC_FILES} ${HEADER_FILES})

# Link OpenMP if available
if(OpenMP_CXX_FOUND)
    target_link_libraries(REEF2FAST PUBLIC OpenMP::OpenMP_CXX)
endif()