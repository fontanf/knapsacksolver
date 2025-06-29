cmake_minimum_required(VERSION 3.15.0)

project(KnapsackSolver LANGUAGES CXX)

# Build options.
option(KNAPSACKSOLVER_BUILD_MAIN "Build main" ON)
option(KNAPSACKSOLVER_BUILD_TEST "Build the unit tests" ON)

# Avoid FetchContent warning.
cmake_policy(SET CMP0135 NEW)

# Require C++11.
set(CMAKE_CXX_STANDARD 11)

# Enable output of compile commands during generation.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add sub-directories.
add_subdirectory(extern)
add_subdirectory(src)
if(KNAPSACKSOLVER_BUILD_TEST)
  add_subdirectory(test)
endif()
