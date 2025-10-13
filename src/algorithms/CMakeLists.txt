add_library(KnapsackSolver_upper_bound_dantzig)
target_sources(KnapsackSolver_upper_bound_dantzig PRIVATE
    upper_bound_dantzig.cpp)
target_include_directories(KnapsackSolver_upper_bound_dantzig PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(KnapsackSolver_upper_bound_dantzig PUBLIC
    KnapsackSolver)
add_library(KnapsackSolver::upper_bound_dantzig ALIAS KnapsackSolver_upper_bound_dantzig)

add_library(KnapsackSolver_greedy)
target_sources(KnapsackSolver_greedy PRIVATE
    greedy.cpp)
target_include_directories(KnapsackSolver_greedy PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(KnapsackSolver_greedy PUBLIC
    KnapsackSolver)
add_library(KnapsackSolver::greedy ALIAS KnapsackSolver_greedy)

find_package(Threads)
add_library(KnapsackSolver_dynamic_programming_bellman)
target_sources(KnapsackSolver_dynamic_programming_bellman PRIVATE
    dynamic_programming_bellman.cpp)
target_include_directories(KnapsackSolver_dynamic_programming_bellman PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(KnapsackSolver_dynamic_programming_bellman PUBLIC
    KnapsackSolver
    KnapsackSolver_upper_bound_dantzig
    KnapsackSolver_greedy
    Threads::Threads)
add_library(KnapsackSolver::dynamic_programming_bellman ALIAS KnapsackSolver_dynamic_programming_bellman)

add_library(KnapsackSolver_dynamic_programming_primal_dual)
target_sources(KnapsackSolver_dynamic_programming_primal_dual PRIVATE
    dynamic_programming_primal_dual.cpp)
target_include_directories(KnapsackSolver_dynamic_programming_primal_dual PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(KnapsackSolver_dynamic_programming_primal_dual PUBLIC
    KnapsackSolver
    KnapsackSolver_upper_bound_dantzig
    KnapsackSolver_greedy)
add_library(KnapsackSolver::dynamic_programming_primal_dual ALIAS KnapsackSolver_dynamic_programming_primal_dual)
