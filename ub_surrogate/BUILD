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

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = ["//lib:tester", "//ub_surrogate:libsurrogate", "//opt_minknap:libminknap"],
        timeout = "moderate",
)

