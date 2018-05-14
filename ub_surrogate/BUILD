cc_library(
        name = "libsurrogate",
        hdrs = ["surrogate.hpp"],
        srcs = ["surrogate.cpp"],
        visibility = ["//visibility:public"],
        deps = [
                "//lib:cpp",
                "//ub_dantzig:libdantzig"
        ],
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

