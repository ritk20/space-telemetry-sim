cmake_minimum_required(VERSION 3.10)
project(SpaceTelemetrySim)

# Set C++ Standard to 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

# Include directories
include_directories(include)

# Define source files common to both executables
set(COMMON_SOURCES
    src/buffer.cpp
    src/sensors.cpp
    src/transmitter.cpp
    src/ground_station.cpp
    src/compression.cpp
)

# Executable for the main simulation
add_executable(sim src/main.cpp ${COMMON_SOURCES})
target_link_libraries(sim Threads::Threads ZLIB::ZLIB)

# Executable for the test simulation
add_executable(test_sim src/test_main.cpp ${COMMON_SOURCES})
target_link_libraries(test_sim Threads::Threads ZLIB::ZLIB)