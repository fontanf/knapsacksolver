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

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = ["//lib:tester", "//lb_greedynlogn:libgreedynlogn", "//opt_minknap:libminknap"],
        timeout = "moderate",
)

