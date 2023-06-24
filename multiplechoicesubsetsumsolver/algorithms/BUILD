load("//knapsacksolver:variables.bzl", "STDCPP")

cc_library(
        name = "dynamic_programming_bellman",
        hdrs = ["dynamic_programming_bellman.hpp"],
        srcs = ["dynamic_programming_bellman.cpp"],
        deps = ["//multiplechoicesubsetsumsolver:multiplechoicesubsetsumsolver"],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_library(
        name = "algorithms",
        hdrs = ["algorithms.hpp"],
        srcs = ["algorithms.cpp"],
        deps = [
                ":dynamic_programming_bellman",
                "@boost//:program_options",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)
