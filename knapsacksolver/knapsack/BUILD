load("//knapsacksolver:variables.bzl", "STDCPP")

cc_library(
        name = "knapsacksolver",
        hdrs = [
                "instance.hpp",
                "instance_builder.hpp",
                "solution.hpp",
                "algorithm_formatter.hpp",
                "sort.hpp",
                "upper_bound.hpp",
        ],
        srcs = [
                "instance.cpp",
                "instance_builder.cpp",
                "solution.cpp",
                "algorithm_formatter.cpp",
                "sort.cpp",
                "upper_bound.cpp",
        ],
        deps = [
                "@optimizationtools//optimizationtools/utils:utils",
                "@optimizationtools//optimizationtools/containers:containers",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [
                "//knapsacksolver/knapsack/algorithms:dynamic_programming_bellman",
                "//knapsacksolver/knapsack/algorithms:dynamic_programming_primal_dual",
                "@boost//:program_options",
        ],
        copts = STDCPP,
)

cc_library(
        name = "generator",
        hdrs = ["generator.hpp"],
        srcs = ["generator.cpp"],
        deps = [
                ":knapsacksolver",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_library(
        name = "tests",
        hdrs = ["tests.hpp"],
        srcs = ["tests.cpp"],
        deps = [
                ":knapsacksolver",
                "@googletest//:gtest_main",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)
