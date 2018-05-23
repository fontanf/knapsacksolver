cc_library(
        name = "libminknap",
        hdrs = ["minknap.hpp"],
        srcs = ["minknap.cpp", "minknap_part.cpp"],
        deps = [
                "//lib:cpp",
                "//lb_greedy:libgreedy",
                "//lb_greedynlogn:libgreedynlogn",
                "//ub_dembo:libdembo",
                "//ub_surrogate:libsurrogate",
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/opt_minknap/",
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [":libminknap"],
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                ":libminknap",
                "//lib:tester",
                "//opt_bellman:libbellman",
        ],
        data = [
                ":main",
                "//data_tests:data",
        ],
        timeout = "moderate",
)
