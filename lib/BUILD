cc_library(
        name = "cpp",
        hdrs = ["instance.hpp", "solution.hpp", "binary_solution.hpp"],
        srcs = ["instance.cpp", "solution.cpp"],
        visibility = ["//visibility:public"],
        deps = [],
)

cc_library(
        name = "cppsolver",
        hdrs = ["solver.hpp"],
        srcs = ["solver.cpp"],
        visibility = ["//visibility:public"],
        deps = [
                ":cpp", 
                "//ub_dantzig:libdantzig",
                "//ub_surrogate:libsurrogate",
                "//lb_greedy:libgreedy",
                "//lb_greedynlogn:libgreedynlogn",
        ],
)

cc_library(
        name = "hpp",
        hdrs = ["instance.hpp", "solution.hpp"],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "tester",
        hdrs = ["tester.hpp"],
        srcs = ["tester.cpp"],
        visibility = ["//visibility:public"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
        ],
        deps = [
                ":cpp", 
                "@googletest//:gtest_main",
        ],
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                ":cpp",
                "//lb_greedy:libgreedy",
                "@googletest//:gtest_main",
        ],
        data = [
                "@KnapsackPisingerInstances//:data",
        ],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options",
                "-lboost_regex",
        ],
        timeout = "long",
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
