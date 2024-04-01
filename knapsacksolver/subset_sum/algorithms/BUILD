load("//knapsacksolver:variables.bzl", "STDCPP")

cc_library(
        name = "dynamic_programming_bellman",
        hdrs = ["dynamic_programming_bellman.hpp"],
        srcs = ["dynamic_programming_bellman.cpp"],
        deps = ["//knapsacksolver/subset_sum:subset_sum"],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_library(
        name = "dynamic_programming_balancing",
        hdrs = ["dynamic_programming_balancing.hpp"],
        srcs = ["dynamic_programming_balancing.cpp"],
        deps = ["//knapsacksolver/subset_sum:subset_sum"],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)
