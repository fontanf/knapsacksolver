load("//knapsacksolver:variables.bzl", "STDCPP")

cc_library(
        name = "multiplechoicesubsetsumsolver",
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
                "//knapsacksolver/multiplechoicesubsetsum/algorithms:dynamic_programming_bellman",
                "@boost//:program_options",
        ],
        copts = STDCPP,
)
