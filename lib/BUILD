cc_library(
        name = "cpp",
        hdrs = ["instance.hpp", "solution.hpp"],
        srcs = ["instance.cpp", "solution.cpp"],
        visibility = ["//visibility:public"],
        deps = ["//ub_dantzig:libdantzig"],
)

cc_library(
        name = "hpp",
        hdrs = ["instance.hpp", "solution.hpp"],
        visibility = ["//visibility:public"],
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                ":cpp", 
                "@googletest//:gtest_main",
        ],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
        timeout = "short",
)

cc_binary(
        name = "sort_bench",
        srcs = ["sort_bench.cpp"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
        deps = ["//lib:cpp"],
)
