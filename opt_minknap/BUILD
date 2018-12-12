cc_library(
        name = "libminknap",
        hdrs = ["minknap.hpp"],
        srcs = ["minknap.cpp"],
        deps = [
                "//lib:cpp",
                "//lb_greedy:libgreedy",
                "//lb_greedynlogn:libgreedynlogn",
                "//ub_dembo:libdembo",
                "//ub_surrogate:libsurrogate",
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/opt_minknap/",
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = ["//lib:tester", "//opt_minknap:libminknap", "//opt_bellman:libbellman"],
        timeout = "moderate",
)

