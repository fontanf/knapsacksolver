add_library(KnapsackSolver)
target_sources(KnapsackSolver PRIVATE
    instance.cpp
    instance_builder.cpp
    solution.cpp
    algorithm_formatter.cpp
    sort.cpp
    upper_bound.cpp)
target_include_directories(KnapsackSolver PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(KnapsackSolver PUBLIC
    OptimizationTools::utils
    OptimizationTools::containers)
add_library(KnapsackSolver ALIAS KnapsackSolver)

add_subdirectory(algorithms)

if(KNAPSACKSOLVER_BUILD_MAIN)
    add_executable(KnapsackSolver_main)
    target_sources(KnapsackSolver_main PRIVATE
        main.cpp)
    target_link_libraries(KnapsackSolver_main PUBLIC
        KnapsackSolver_greedy
        KnapsackSolver_dynamic_programming_bellman
        KnapsackSolver_dynamic_programming_primal_dual
        Boost::program_options)
    set_target_properties(KnapsackSolver_main PROPERTIES OUTPUT_NAME "knapsacksolver")
    install(TARGETS KnapsackSolver_main)
endif()

add_library(KnapsackSolver_generator)
target_sources(KnapsackSolver_generator PRIVATE
    generator.cpp)
target_link_libraries(KnapsackSolver_generator PUBLIC
    KnapsackSolver)
add_library(KnapsackSolver::generator ALIAS KnapsackSolver_generator)

if(KNAPSACKSOLVER_BUILD_TEST)
    add_library(KnapsackSolver_tests)
    target_sources(KnapsackSolver_tests PRIVATE
        tests.cpp)
    target_link_libraries(KnapsackSolver_tests PUBLIC
        KnapsackSolver
        Boost::filesystem
        GTest::gtest_main)
    add_library(KnapsackSolver::tests ALIAS KnapsackSolver_tests)
endif()
