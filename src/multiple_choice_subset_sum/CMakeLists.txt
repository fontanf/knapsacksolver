add_library(KnapsackSolver_multiple_choice_subset_sum)
target_sources(KnapsackSolver_multiple_choice_subset_sum PRIVATE
    instance.cpp
    instance_builder.cpp
    solution.cpp
    algorithm_formatter.cpp)
target_include_directories(KnapsackSolver_multiple_choice_subset_sum PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(KnapsackSolver_multiple_choice_subset_sum PUBLIC
    OptimizationTools::utils)
add_library(KnapsackSolver::multiple_choice_subset_sum ALIAS KnapsackSolver_multiple_choice_subset_sum)

add_subdirectory(algorithms)

if(KNAPSACKSOLVER_BUILD_MAIN)
  add_executable(KnapsackSolver_multiple_choice_subset_sum_main)
  target_sources(KnapsackSolver_multiple_choice_subset_sum_main PRIVATE
      main.cpp)
  target_link_libraries(KnapsackSolver_multiple_choice_subset_sum_main PUBLIC
      KnapsackSolver_multiple_choice_subset_sum_dynamic_programming_bellman
      Boost::program_options)
  set_target_properties(KnapsackSolver_multiple_choice_subset_sum_main PROPERTIES OUTPUT_NAME "knapsacksolver_multiple_choice_subset_sum")
  install(TARGETS KnapsackSolver_multiple_choice_subset_sum_main)
endif()
