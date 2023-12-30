load("//knapsacksolver:variables.bzl", "STDCPP")

cc_library(
        name = "subsetsumsolver",
        hdrs = [
                "instance.hpp",
                "instance_builder.hpp",
                "solution.hpp",
                "algorithm_formatter.hpp",
        ],
        srcs = [
                "instance.cpp",
                "instance_builder.cpp",
                "solution.cpp",
                "algorithm_formatter.cpp",
        ],
        deps = [
                "@optimizationtools//optimizationtools/utils:utils",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [
                "//knapsacksolver/subsetsum/algorithms:dynamic_programming_bellman",
                "//knapsacksolver/subsetsum/algorithms:dynamic_programming_balancing",
                "@boost//:program_options",
        ],
        copts = STDCPP,
)


cc_library(
        name = "generator",
        hdrs = ["generator.hpp"],
        srcs = ["generator.cpp"],
        deps = [
                ":subsetsumsolver",
        ],
        visibility = ["//visibility:public"],
)

cc_binary(
        name = "generator_main",
        srcs = ["generator_main.cpp"],
        deps = [
                "@boost//:program_options",
                ":generator",
        ],
)
