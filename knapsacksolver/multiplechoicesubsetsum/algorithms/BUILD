load("//knapsacksolver:variables.bzl", "STDCPP")

cc_library(
        name = "dynamic_programming_bellman",
        hdrs = ["dynamic_programming_bellman.hpp"],
        srcs = ["dynamic_programming_bellman.cpp"],
        deps = ["//knapsacksolver/multiplechoicesubsetsum:multiplechoicesubsetsumsolver"],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)
