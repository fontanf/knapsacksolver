cc_library(
        name = "libdantzig",
        hdrs = ["dantzig.hpp"],
        srcs = ["dantzig.cpp"],
        deps = ["//lib:cpp"],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/ub_dantzig/",
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [":libdantzig"],
)
