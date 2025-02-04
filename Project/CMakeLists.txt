# CMakeLists.txt for Standalone ASIO

cmake_minimum_required(VERSION 3.10)
project(AsioProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find and include ASIO
find_path(ASIO_INCLUDE_DIR asio.hpp)

if (NOT ASIO_INCLUDE_DIR)
    message(FATAL_ERROR "ASIO not found")
endif()

# Collect all source files
file(GLOB_RECURSE SOURCES
        src/*.cpp
)

# Collect all header files (if needed)
file(GLOB_RECURSE HEADERS
        include/*.hpp
        include/*.h
)

# Include ASIO headers
include_directories(${ASIO_INCLUDE_DIR})

# Add the executable with all sources
add_executable(AsioExample ${SOURCES} ${HEADERS}
        src/HttpClient.cpp
        include/HttpClient.hpp)


# Link with pthread if needed (required for multithreading with ASIO)
target_link_libraries(AsioExample pthread)