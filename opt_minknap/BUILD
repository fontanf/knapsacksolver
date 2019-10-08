cc_library(
        name = "minknap",
        hdrs = ["minknap.hpp"],
        srcs = ["minknap.cpp"],
        deps = [
                "//lib:cpp",
                "//lb_greedy:greedy",
                "//lb_greedynlogn:greedynlogn",
                "//ub_dembo:dembo",
                "//ub_surrogate:surrogate",
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/opt_minknap/",
)

cc_library(
        name = "minknap_hpp",
        hdrs = ["minknap.hpp"],
        srcs = [],
        deps = [],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/opt_minknap/",
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                "//lib:tester",
                "//opt_minknap:minknap",
                "//opt_bellman:bellman"
        ],
        timeout = "long",
)

