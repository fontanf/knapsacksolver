cc_library(
        name = "libbalknap",
        hdrs = ["balknap.hpp"],
        srcs = ["balknap.cpp"],
        deps = [
                "//lib:cpp",
                "//lib:libbounds",
                "//lb_greedy:libgreedy",
                "//lb_greedynlogn:libgreedynlogn",
                "//ub_dembo:libdembo",
                "//ub_surrogate:libsurrogate",
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/opt_balknap/",
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [":libbalknap"],
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                "//lib:tester",
                ":libbalknap",
                "//lib:libgenerator",
                "//opt_minknap:libminknap",
        ],
        data = [
                ":main",
                "//data_tests:data",
        ],
        timeout = "long",
)
