cc_library(
        name = "libastar",
        hdrs = ["astar.hpp"],
        srcs = ["astar.cpp"],
        deps = [
                "//lib:cpp",
                "//ub_dembo:libdembo",
                "//lb_greedy:libgreedy",
                "//lb_greedynlogn:libgreedynlogn",
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/opt_astar/",
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = ["//lib:tester", "//opt_astar:libastar", "//opt_minknap:libminknap"],
        timeout = "moderate",
)

