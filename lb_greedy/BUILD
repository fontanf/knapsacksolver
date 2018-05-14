cc_library(
        name = "libgreedy",
        hdrs = ["greedy.hpp"],
        srcs = ["greedy.cpp"],
        deps = ["//lib:cpp"],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/lb_greedy/",
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [":libgreedy"],
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                ":libgreedy",
                "@googletest//:gtest_main",
        ],
        timeout = "short",
)

