cc_library(
        name = "libbab",
        hdrs = ["bab.hpp"],
        srcs = ["bab.cpp"],
        visibility = ["//visibility:public"],
        deps = [
                "//lib:cpp",
                "//ub_dembo:libdembo",
        ],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [":libbab", "//lib:cpp", "//lb_greedy:libgreedy"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
        ],
        deps = [
                "//lib:tester",
                "//lib:cpp",
        ],
        data = [
                ":main",
                "//data_tests:data",
        ],
        timeout = "short",
)

