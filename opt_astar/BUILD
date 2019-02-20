cc_library(
        name = "astar",
        hdrs = ["astar.hpp"],
        srcs = ["astar.cpp"],
        deps = [
                "//lib:cpp",
                "//ub_dembo:dembo",
                "//lb_greedy:greedy",
                "//lb_greedynlogn:greedynlogn",
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/opt_astar/",
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                "//lib:tester",
                "//opt_astar:astar",
                "//opt_minknap:minknap",
        ],
        timeout = "moderate",
)

