cc_library(
        name = "libbalknap",
        hdrs = ["balknap.hpp"],
        srcs = ["balknap_array.cpp", "balknap_list.cpp"],
        visibility = ["//visibility:public"],
        deps = [
                "//lib:cpp",
                "//lb_greedy:libgreedy",
                "//lb_greedynlogn:libgreedynlogn",
                "//ub_dembo:libdembo",
                "//ub_surrogate:libsurrogate",
        ],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [":libbalknap"],
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = ["//lib:tester"],
        data = [
                ":main",
                "//data_tests:data",
        ],
        timeout = "short",
)
