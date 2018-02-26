cc_library(
        name = "libls",
        hdrs = ["ls.hpp"],
        srcs = ["ls.cpp"],
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
                ":libls",
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
                ":libls",
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

