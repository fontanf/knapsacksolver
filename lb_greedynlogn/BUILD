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
        deps = [
                ":libgreedynlogn",
                "//lib:cpp"
        ],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                ":libgreedynlogn",
                "//lib:cpp",
                "@googletest//:gtest_main",
        ],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
        timeout = "short",
)

