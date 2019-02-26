cc_library(
        name = "dpprofits",
        hdrs = ["dpprofits.hpp"],
        srcs = ["dpprofits.cpp"],
        deps = [
                "//lib:cpp",
                "//ub_dembo:dembo"
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/opt_dpprofits/",
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                "//lib:tester",
                "//opt_dpprofits:dpprofits",
                "//opt_bellman:bellman",
        ],
        timeout = "moderate",
)

