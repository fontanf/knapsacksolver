cc_library(
        name = "libbab",
        hdrs = ["bab.hpp"],
        srcs = ["bab.cpp"],
        deps = ["//lib:cpp", "//ub_dembo:libdembo"],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/opt_bab/",
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = ["//lib:tester", "//opt_bab:libbab", "//opt_minknap:libminknap"],
        timeout = "moderate",
)

