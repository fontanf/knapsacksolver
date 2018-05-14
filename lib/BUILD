cc_library(
        name = "cpp",
        hdrs = [
                "instance.hpp",
                "solution.hpp",
                "part_solution_1.hpp",
                "part_solution_2.hpp"
        ],
        srcs = ["instance.cpp", "solution.cpp"],
        deps = [
                "@benchtools//:info",
        ],
        linkopts = [
                "-lboost_iostreams",
                "-lboost_serialization",
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options",
                "-lboost_regex",
        ],
        include_prefix = "knapsack/lib/",
        visibility = ["//visibility:public"],
)

cc_library(
        name = "hpp",
        hdrs = [
                "instance.hpp",
                "solution.hpp"
        ],
        deps = ["@benchtools//:info",],
        include_prefix = "knapsack/lib/",
        visibility = ["//visibility:public"],
)

cc_library(
        name = "libgenerator",
        hdrs = ["generator.hpp"],
        srcs = ["generator.cpp"],
        deps = [":cpp"],
        visibility = ["//visibility:public"],
)

cc_binary(
        name = "generator_main",
        srcs = ["generator.cpp", "generator.hpp"],
        deps = ["//lib:cpp"],
)

cc_library(
        name = "tester",
        hdrs = ["tester.hpp"],
        srcs = ["tester.cpp"],
        deps = [
                ":cpp", 
                "@googletest//:gtest_main",
        ],
        visibility = ["//visibility:public"],
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
        timeout = "long",
)

cc_binary(
        name = "sort_bench",
        srcs = ["sort_bench.cpp"],
        deps = ["//lib:cpp"],
)
