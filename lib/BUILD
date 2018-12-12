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
        name = "libgenerator",
        hdrs = ["generator.hpp"],
        srcs = ["generator.cpp"],
        deps = ["//lib:cpp"],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/lib/",
)

cc_binary(
        name = "generator_main",
        srcs = ["generator_main.cpp"],
        deps = [":libgenerator"],
)

cc_library(
        name = "tester",
        hdrs = ["tester.hpp"],
        srcs = ["tester.cpp"],
        deps = [
                ":cpp", 
                ":libgenerator",
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
                "//lb_greedy:libgreedy",
                "@googletest//:gtest_main",
        ],
        data = [
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
                "//lb_greedy:libgreedy",
                "//lb_greedynlogn:libgreedynlogn",
                "//ub_dembo:libdembo",
                "//ub_surrogate:libsurrogate",
                "//opt_bab:libbab",
                "//opt_astar:libastar",
                "//opt_expknap:libexpknap",
                "//opt_bellman:libbellman",
                "//opt_dpprofits:libdpprofits",
                "//opt_balknap:libbalknap",
                "//opt_minknap:libminknap",
                ":libgenerator",
        ],
)

cc_binary(
        name = "test_main",
        srcs = ["test_main.cpp"],
        deps = [
                "//lb_greedy:libgreedy",
                "//lb_greedynlogn:libgreedynlogn",
                "//ub_dembo:libdembo",
                "//ub_surrogate:libsurrogate",
                "//opt_bab:libbab",
                "//opt_astar:libastar",
                "//opt_expknap:libexpknap",
                "//opt_bellman:libbellman",
                "//opt_dpprofits:libdpprofits",
                "//opt_balknap:libbalknap",
                "//opt_minknap:libminknap",
                ":libgenerator",
                ":tester",
        ],
)

