load("//knapsacksolver:variables.bzl", "STDCPP")

cc_library(
        name = "subsetsumsolver",
        hdrs = [
                "instance.hpp",
                "solution.hpp",
                "algorithms/dynamic_programming_bellman.hpp",
        ],
        srcs = [
                "instance.cpp",
                "solution.cpp",
                "algorithms/dynamic_programming_bellman.cpp",
        ],
        deps = [
                "@optimizationtools//optimizationtools/utils:info",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_library(
        name = "algorithms",
        hdrs = ["algorithms.hpp"],
        srcs = ["algorithms.cpp"],
        deps = [
                ":subsetsumsolver",
                "@boost//:program_options",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [
                ":algorithms",
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

