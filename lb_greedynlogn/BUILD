cc_library(
        name = "libgreedynlogn",
        hdrs = ["greedynlogn.hpp"],
        srcs = ["greedynlogn.cpp"],
        deps = [
                "//lib:cpp",
                "//lb_greedy:libgreedy",
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/lb_greedynlogn/",
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [":libgreedynlogn"],
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                ":libgreedynlogn",
                "@googletest//:gtest_main",
        ],
        timeout = "short",
)

