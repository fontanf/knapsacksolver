cc_library(
        name = "libbabstar",
        hdrs = ["bab.hpp"],
        srcs = ["bab.cpp"],
        deps = [
                "//lib:cpp",
                "//ub_dembo:libdembo",
                "//lb_greedy:libgreedy",
                "//lb_greedynlogn:libgreedynlogn",
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/opt_babstar/",
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [":libbabstar"],
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                "//lib:tester",
                ":libbabstar",
                "//lib:libgenerator",
                "//opt_minknap:libminknap",
        ],
        data = [
                ":main",
                "//data_tests:data",
        ],
        timeout = "moderate",
)

