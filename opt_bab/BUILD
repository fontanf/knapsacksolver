cc_library(
        name = "bab",
        hdrs = ["bab.hpp"],
        srcs = ["bab.cpp"],
        deps = [
                "//lib:cpp",
                "//ub_dembo:dembo",
                "//ub_dantzig:dantzig",
                "//lb_greedynlogn:greedynlogn",
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/opt_bab/",
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                "//lib:tester",
                "//opt_bab:bab",
                "//opt_minknap:minknap",
        ],
        timeout = "moderate",
)

