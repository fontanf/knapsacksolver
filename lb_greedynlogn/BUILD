cc_library(
        name = "greedynlogn",
        hdrs = ["greedynlogn.hpp"],
        srcs = ["greedynlogn.cpp"],
        deps = [
                "//lib:cpp",
                "//lb_greedy:greedy",
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/lb_greedynlogn/",
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                "//lib:tester",
                "//lb_greedynlogn:greedynlogn",
                "//opt_minknap:minknap"
        ],
        timeout = "moderate",
)

