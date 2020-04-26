load("//knapsacksolver:variables.bzl", "STDCPP")
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")

pybind_extension(
        name = "knapsacksolver",
        srcs = ["knapsacksolver.cpp"],
        deps = [
                "//knapsacksolver/algorithms:algorithms",
        ],
        copts = STDCPP,
)

