cc_library(
        name = "libgreedy",
        hdrs = ["greedy.hpp"],
        srcs = ["greedy.cpp"],
        visibility = ["//visibility:public"],
        deps = ["//lib:cpp"],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [":libgreedy"],
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                ":libgreedy",
                "@googletest//:gtest_main",
        ],
        timeout = "short",
)

