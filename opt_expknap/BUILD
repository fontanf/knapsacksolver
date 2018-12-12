cc_library(
        name = "libexpknap",
        hdrs = ["expknap.hpp"],
        srcs = ["expknap.cpp"],
        deps = [
                "//lib:cpp",
                "//lb_greedy:libgreedy",
                "//lb_greedynlogn:libgreedynlogn",
                "//ub_dembo:libdembo",
                "//ub_surrogate:libsurrogate",
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/opt_expknap/",
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = ["//lib:tester", "//opt_expknap:libexpknap", "//opt_minknap:libminknap"],
        timeout = "moderate",
)

