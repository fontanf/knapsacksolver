cc_library(
        name = "knapsacksolver",
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
                "//knapsacksolver/algorithms:dembo",
                "@benchtools//:info",
        ],
        linkopts = select({
                "@bazel_tools//src/conditions:windows": [],
                "//conditions:default":                 ["-lpthread"],
        }),
        copts = select({
                "@bazel_tools//src/conditions:windows": ['/std:c++latest'],
                "//conditions:default":                 ["-std=c++14"],
        }),
        visibility = ["//visibility:public"],
)

cc_library(
        name = "knapsacksolver_hpp",
        hdrs = [
                "instance.hpp",
                "solution.hpp"
        ],
        deps = ["@benchtools//:info",],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "generator",
        hdrs = ["generator.hpp"],
        srcs = ["generator.cpp"],
        deps = [
                ":knapsacksolver",
                "//knapsacksolver/algorithms:minknap"
        ],
        visibility = ["//visibility:public"],
)

cc_binary(
        name = "generator_main",
        srcs = ["generator_main.cpp"],
        deps = [
                "@boost//:program_options",
                ":generator",
        ],
)

cc_library(
        name = "tester",
        hdrs = ["tester.hpp"],
        srcs = ["tester.cpp"],
        deps = [
                ":knapsacksolver",
                ":generator",
                "@googletest//:gtest_main",
        ],
        visibility = ["//visibility:public"],
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = [
                ":knapsacksolver",
                "//knapsacksolver/algorithms:greedy",
                "@googletest//:gtest_main",
        ],
        timeout = "long",
)

cc_binary(
        name = "sort_bench",
        srcs = ["sort_bench.cpp"],
        deps = [":knapsacksolver"],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [
                "@boost//:program_options",
                "//knapsacksolver/algorithms:algorithms",
        ],
        copts = select({
                "@bazel_tools//src/conditions:windows": ['/std:c++latest'],
                "//conditions:default":                 ["-std=c++14"],
        }),
)

cc_binary(
        name = "bench",
        srcs = ["bench.cpp"],
        deps = [
                "@boost//:program_options",
                "//knapsacksolver/algorithms:algorithms",
                ":generator",
        ],
        copts = select({
                "@bazel_tools//src/conditions:windows": ['/std:c++latest'],
                "//conditions:default":                 ["-std=c++14"],
        }),
)

cc_binary(
        name = "test_main",
        srcs = ["test_main.cpp"],
        deps = [
                "//knapsacksolver/algorithms:algorithms",
                ":generator",
                ":tester",
        ],
        copts = select({
                "@bazel_tools//src/conditions:windows": ['/std:c++latest'],
                "//conditions:default":                 ["-std=c++14"],
        }),
)

