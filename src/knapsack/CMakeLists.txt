add_library(KnapsackSolver_knapsack)
target_sources(KnapsackSolver_knapsack PRIVATE
    instance.cpp
    instance_builder.cpp
    solution.cpp
    algorithm_formatter.cpp
    sort.cpp
    upper_bound.cpp)
target_include_directories(KnapsackSolver_knapsack PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(KnapsackSolver_knapsack PUBLIC
    OptimizationTools::utils
    OptimizationTools::containers)
add_library(KnapsackSolver::knapsack ALIAS KnapsackSolver_knapsack)

add_subdirectory(algorithms)

if(KNAPSACKSOLVER_BUILD_MAIN)
  add_executable(KnapsackSolver_knapsack_main)
  target_sources(KnapsackSolver_knapsack_main PRIVATE
      main.cpp)
  target_link_libraries(KnapsackSolver_knapsack_main PUBLIC
      KnapsackSolver_knapsack_greedy
      KnapsackSolver_knapsack_dynamic_programming_bellman
      KnapsackSolver_knapsack_dynamic_programming_primal_dual
      Boost::program_options)
  set_target_properties(KnapsackSolver_knapsack_main PROPERTIES OUTPUT_NAME "knapsacksolver_knapsack")
  install(TARGETS KnapsackSolver_knapsack_main)
endif()

add_library(KnapsackSolver_knapsack_generator)
target_sources(KnapsackSolver_knapsack_generator PRIVATE
    generator.cpp)
target_link_libraries(KnapsackSolver_knapsack_generator PUBLIC
    KnapsackSolver_knapsack)
add_library(KnapsackSolver::knapsack::generator ALIAS KnapsackSolver_knapsack_generator)

if(KNAPSACKSOLVER_BUILD_TEST)
  add_library(KnapsackSolver_knapsack_tests)
  target_sources(KnapsackSolver_knapsack_tests PRIVATE
      tests.cpp)
  target_link_libraries(KnapsackSolver_knapsack_tests PUBLIC
      KnapsackSolver_knapsack
      Boost::filesystem
      GTest::gtest_main)
  add_library(KnapsackSolver::knapsack::tests ALIAS KnapsackSolver_knapsack_tests)
endif()
