load("//knapsacksolver:variables.bzl", "STDCPP")

cc_library(
        name = "multiplechoicesubsetsumsolver",
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
                "@optimizationtools//optimizationtools/utils:utils",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = ["//multiplechoicesubsetsumsolver/algorithms:algorithms"],
        copts = STDCPP,
)

