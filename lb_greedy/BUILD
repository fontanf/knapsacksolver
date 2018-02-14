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
        deps = [
                ":libgreedy",
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
                ":libgreedy",
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

