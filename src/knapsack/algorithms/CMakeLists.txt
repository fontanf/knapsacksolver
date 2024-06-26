add_library(KnapsackSolver_knapsack_upper_bound_dantzig)
target_sources(KnapsackSolver_knapsack_upper_bound_dantzig PRIVATE
    upper_bound_dantzig.cpp)
target_include_directories(KnapsackSolver_knapsack_upper_bound_dantzig PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(KnapsackSolver_knapsack_upper_bound_dantzig PUBLIC
    KnapsackSolver_knapsack)
add_library(KnapsackSolver::knapsack::upper_bound_dantzig ALIAS KnapsackSolver_knapsack_upper_bound_dantzig)

add_library(KnapsackSolver_knapsack_greedy)
target_sources(KnapsackSolver_knapsack_greedy PRIVATE
    greedy.cpp)
target_include_directories(KnapsackSolver_knapsack_greedy PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(KnapsackSolver_knapsack_greedy PUBLIC
    KnapsackSolver_knapsack)
add_library(KnapsackSolver::knapsack::greedy ALIAS KnapsackSolver_knapsack_greedy)

add_library(KnapsackSolver_knapsack_dynamic_programming_bellman)
target_sources(KnapsackSolver_knapsack_dynamic_programming_bellman PRIVATE
    dynamic_programming_bellman.cpp)
target_include_directories(KnapsackSolver_knapsack_dynamic_programming_bellman PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(KnapsackSolver_knapsack_dynamic_programming_bellman PUBLIC
    KnapsackSolver_knapsack
    KnapsackSolver_knapsack_upper_bound_dantzig
    KnapsackSolver_knapsack_greedy)
add_library(KnapsackSolver::knapsack::dynamic_programming_bellman ALIAS KnapsackSolver_knapsack_dynamic_programming_bellman)

add_library(KnapsackSolver_knapsack_dynamic_programming_primal_dual)
target_sources(KnapsackSolver_knapsack_dynamic_programming_primal_dual PRIVATE
    dynamic_programming_primal_dual.cpp)
target_include_directories(KnapsackSolver_knapsack_dynamic_programming_primal_dual PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(KnapsackSolver_knapsack_dynamic_programming_primal_dual PUBLIC
    KnapsackSolver_knapsack
    KnapsackSolver_knapsack_upper_bound_dantzig
    KnapsackSolver_knapsack_greedy)
add_library(KnapsackSolver::knapsack::dynamic_programming_primal_dual ALIAS KnapsackSolver_knapsack_dynamic_programming_primal_dual)
