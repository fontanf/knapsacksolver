# Enable FetchContent.
include(FetchContent)

# Fetch boost.
set(BOOST_INCLUDE_LIBRARIES thread filesystem system program_options)
set(BOOST_ENABLE_CMAKE ON)
include(FetchContent)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.84.0
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(Boost)

# Fetch fontanf/optimizationtools.
FetchContent_Declare(
    optimizationtools
    GIT_REPOSITORY https://github.com/fontanf/optimizationtools.git
    GIT_TAG 9007a5ce556d53d8179deafedf9a171a7b9cb8a4)
    #SOURCE_DIR "${PROJECT_SOURCE_DIR}/../optimizationtools/")
FetchContent_MakeAvailable(optimizationtools)
