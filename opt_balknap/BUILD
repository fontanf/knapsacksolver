cc_library(
        name = "libbalknap",
        hdrs = ["balknap.hpp"],
        srcs = ["balknap.cpp"],
        deps = [
                "//lib:cpp",
                "//lb_greedy:libgreedy",
                "//lb_greedynlogn:libgreedynlogn",
                "//ub_dembo:libdembo",
                "//ub_surrogate:libsurrogate",
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/opt_balknap/",
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = ["//lib:tester", "//opt_balknap:libbalknap", "//opt_minknap:libminknap"],
        timeout = "moderate",
)

