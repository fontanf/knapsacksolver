cc_library(
        name = "bellman",
        hdrs = ["bellman.hpp"],
        srcs = ["bellman_array.cpp", "bellman_list.cpp"],
        deps = [
                "//lib:cpp",
                "//ub_dembo:dembo"
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/opt_bellman/",
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                "//lib:tester",
                "//opt_bellman:bellman"
        ],
        timeout = "moderate",
)

