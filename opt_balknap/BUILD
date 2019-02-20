cc_library(
        name = "balknap",
        hdrs = ["balknap.hpp"],
        srcs = ["balknap.cpp"],
        deps = [
                "//lib:cpp",
                "//lb_greedy:greedy",
                "//lb_greedynlogn:greedynlogn",
                "//ub_dembo:dembo",
                "//ub_surrogate:surrogate",
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/opt_balknap/",
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                "//lib:tester",
                "//opt_balknap:balknap",
                "//opt_minknap:minknap",
        ],
        timeout = "moderate",
)

