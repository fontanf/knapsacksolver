cc_library(
        name = "libsurrogate",
        hdrs = ["surrogate.hpp"],
        srcs = ["surrogate.cpp"],
        visibility = ["//visibility:public"],
        deps = [
                "//lib:hpp",
                "//ub_dantzig:libdantzig"
        ],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [
                ":libsurrogate",
                "//lib:cpp",
                "//lb_greedy:libgreedy",
                "//lb_greedynlogn:libgreedynlogn",
        ],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
)
