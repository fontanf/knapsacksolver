cc_library(
        name = "surrogate",
        hdrs = ["surrogate.hpp"],
        srcs = ["surrogate.cpp"],
        deps = [
                "//lib:cpp",
                "//ub_dantzig:dantzig"
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/ub_surrogate/",
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                "//lib:tester",
                "//ub_surrogate:surrogate",
                "//opt_minknap:minknap",
        ],
        timeout = "moderate",
)

