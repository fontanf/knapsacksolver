load("//knapsacksolver:variables.bzl", "STDCPP")

cc_library(
        name = "dynamic_programming_bellman",
        hdrs = ["dynamic_programming_bellman.hpp"],
        srcs = ["dynamic_programming_bellman.cpp"],
        deps = ["//knapsacksolver/multiple_choice_subset_sum:multiple_choice_subset_sum"],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)
