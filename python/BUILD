load("//knapsacksolver:variables.bzl", "STDCPP")
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")

pybind_extension(
        name = "knapsacksolver",
        srcs = ["knapsacksolver.cpp"],
        deps = [
            "//knapsacksolver/knapsack/algorithms:dynamic_programming_bellman",
            "//knapsacksolver/knapsack/algorithms:dynamic_programming_primal_dual",
        ],
        copts = STDCPP,
)

