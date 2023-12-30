load("//knapsacksolver:variables.bzl", "STDCPP")

cc_library(
        name = "upper_bound_dantzig",
        hdrs = ["upper_bound_dantzig.hpp"],
        srcs = ["upper_bound_dantzig.cpp"],
        deps = ["//knapsacksolver/knapsack:knapsacksolver"],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_library(
        name = "greedy",
        hdrs = ["greedy.hpp"],
        srcs = ["greedy.cpp"],
        deps = ["//knapsacksolver/knapsack:knapsacksolver"],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_library(
        name = "dynamic_programming_bellman",
        hdrs = ["dynamic_programming_bellman.hpp"],
        srcs = ["dynamic_programming_bellman.cpp"],
        deps = [
                "//knapsacksolver/knapsack:knapsacksolver",
                ":upper_bound_dantzig",
                ":greedy",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_test(
        name = "dynamic_programming_bellman_test",
        srcs = ["dynamic_programming_bellman_test.cpp"],
        deps = [
                ":dynamic_programming_bellman",
                "//knapsacksolver/knapsack:tests",
        ],
        copts = STDCPP,
        data = ["//data/knapsack:tests"],
        timeout = "short",
)

cc_library(
        name = "dynamic_programming_primal_dual",
        hdrs = ["dynamic_programming_primal_dual.hpp"],
        srcs = ["dynamic_programming_primal_dual.cpp"],
        deps = [
                "//knapsacksolver/knapsack:knapsacksolver",
                ":upper_bound_dantzig",
                ":greedy",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_test(
        name = "dynamic_programming_primal_dual_test",
        srcs = ["dynamic_programming_primal_dual_test.cpp"],
        deps = [
                ":dynamic_programming_primal_dual",
                ":dynamic_programming_bellman",
                "//knapsacksolver/knapsack:generator",
                "//knapsacksolver/knapsack:tests",
        ],
        copts = STDCPP,
        data = ["//data/knapsack:tests"],
)
