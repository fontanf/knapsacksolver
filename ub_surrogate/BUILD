cc_library(
        name = "libsurrogate",
        hdrs = ["surrogate.hpp"],
        srcs = ["surrogate.cpp"],
        deps = [
                "//lib:cpp",
                "//ub_dantzig:libdantzig"
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/ub_surrogate/",
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [
                ":libsurrogate",
                "//lb_greedy:libgreedy",
                "//lb_greedynlogn:libgreedynlogn",
        ],
)

