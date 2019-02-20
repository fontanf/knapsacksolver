cc_library(
        name = "expknap",
        hdrs = ["expknap.hpp"],
        srcs = ["expknap.cpp"],
        deps = [
                "//lib:cpp",
                "//lb_greedy:greedy",
                "//lb_greedynlogn:greedynlogn",
                "//ub_dembo:dembo",
                "//ub_surrogate:surrogate",
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/opt_expknap/",
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                "//lib:tester",
                "//opt_expknap:expknap",
                "//opt_minknap:minknap",
        ],
        timeout = "moderate",
)

