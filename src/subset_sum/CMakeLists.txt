add_library(KnapsackSolver_subset_sum)
target_sources(KnapsackSolver_subset_sum PRIVATE
    instance.cpp
    instance_builder.cpp
    solution.cpp
    algorithm_formatter.cpp)
target_include_directories(KnapsackSolver_subset_sum PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(KnapsackSolver_subset_sum PUBLIC
    OptimizationTools::utils)
add_library(KnapsackSolver::subset_sum ALIAS KnapsackSolver_subset_sum)

add_subdirectory(algorithms)

add_executable(KnapsackSolver_subset_sum_main)
target_sources(KnapsackSolver_subset_sum_main PRIVATE
    main.cpp)
target_link_libraries(KnapsackSolver_subset_sum_main PUBLIC
    KnapsackSolver_subset_sum_dynamic_programming_bellman
    KnapsackSolver_subset_sum_dynamic_programming_balancing
    KnapsackSolver_subset_sum_dynamic_programming_primal_dual
    Boost::program_options)
set_target_properties(KnapsackSolver_subset_sum_main PROPERTIES OUTPUT_NAME "knapsacksolver_subset_sum" ) 
install(TARGETS KnapsackSolver_subset_sum_main DESTINATION bin)

add_library(KnapsackSolver_subset_sum_generator)
target_sources(KnapsackSolver_subset_sum_generator PRIVATE
    generator.cpp)
target_link_libraries(KnapsackSolver_subset_sum_generator PUBLIC
    KnapsackSolver_subset_sum)
add_library(KnapsackSolver::knapsack::generator ALIAS KnapsackSolver_subset_sum_generator)
