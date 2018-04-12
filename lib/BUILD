cc_library(
        name = "cpp",
        hdrs = [
                "instance.hpp",
                "solution.hpp",
                "part_solution_1.hpp",
                "part_solution_2.hpp"
        ],
        srcs = ["instance.cpp", "solution.cpp"],
        visibility = ["//visibility:public"],
        deps = [
                "@benchtools//:info",
        ],
)

cc_library(
        name = "hpp",
        hdrs = ["instance.hpp", "solution.hpp"],
        deps = ["@benchtools//:info",],
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
                "@knapsack_instances_pisinger//:data",
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
