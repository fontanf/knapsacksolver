cc_library(
        name = "libgreedynlogn",
        hdrs = ["greedynlogn.hpp"],
        srcs = ["greedynlogn.cpp"],
        visibility = ["//visibility:public"],
        deps = [
                "//lib:cpp",
                "//lb_greedy:libgreedy",
        ],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [":libgreedynlogn"],
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                ":libgreedynlogn",
                "@googletest//:gtest_main",
        ],
        timeout = "short",
)

