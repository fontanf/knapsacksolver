load("//knapsacksolver:variables.bzl", "STDCPP")

cc_library(
        name = "subsetsumsolver",
        hdrs = [
                "instance.hpp",
                "solution.hpp",
        ],
        srcs = [
                "instance.cpp",
                "solution.cpp",
        ],
        deps = [
                "@optimizationtools//optimizationtools/utils:info",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = ["//subsetsumsolver/algorithms:algorithms"],
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

