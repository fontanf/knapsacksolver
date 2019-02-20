cc_library(
        name = "cpp",
        hdrs = [
                "instance.hpp",
                "solution.hpp",
                "part_solution_1.hpp",
                "part_solution_2.hpp",
        ],
        srcs = [
                "instance.cpp",
                "solution.cpp"
        ],
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
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/lib/",
)

cc_library(
        name = "hpp",
        hdrs = [
                "instance.hpp",
                "solution.hpp"
        ],
        deps = ["@benchtools//:info",],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/lib/",
)

cc_library(
        name = "generator",
        hdrs = ["generator.hpp"],
        srcs = ["generator.cpp"],
        deps = ["//lib:cpp"],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/lib/",
)

cc_binary(
        name = "generator_main",
        srcs = ["generator_main.cpp"],
        deps = [":generator"],
)

cc_library(
        name = "tester",
        hdrs = ["tester.hpp"],
        srcs = ["tester.cpp"],
        deps = [
                ":cpp", 
                ":generator",
                "@googletest//:gtest_main",
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/lib/",
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                ":cpp",
                "//lb_greedy:greedy",
                "@googletest//:gtest_main",
        ],
        timeout = "long",
)

cc_binary(
        name = "sort_bench",
        srcs = ["sort_bench.cpp"],
        deps = ["//lib:cpp"],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [
                "//lb_greedy:greedy",
                "//lb_greedynlogn:greedynlogn",
                "//ub_dembo:dembo",
                "//ub_surrogate:surrogate",
                "//opt_bab:bab",
                "//opt_astar:astar",
                "//opt_expknap:expknap",
                "//opt_bellman:bellman",
                "//opt_dpprofits:dpprofits",
                "//opt_balknap:balknap",
                "//opt_minknap:minknap",
                ":generator",
        ],
)

cc_binary(
        name = "test_main",
        srcs = ["test_main.cpp"],
        deps = [
                "//lb_greedy:greedy",
                "//lb_greedynlogn:greedynlogn",
                "//ub_dembo:dembo",
                "//ub_surrogate:surrogate",
                "//opt_bab:bab",
                "//opt_astar:astar",
                "//opt_expknap:expknap",
                "//opt_bellman:bellman",
                "//opt_dpprofits:dpprofits",
                "//opt_balknap:balknap",
                "//opt_minknap:minknap",
                ":generator",
                ":tester",
        ],
)

