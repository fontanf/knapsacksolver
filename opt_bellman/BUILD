cc_library(
        name = "libbellman",
        hdrs = ["bellman.hpp"],
        srcs = ["bellman.cpp"],
        visibility = ["//visibility:public"],
        deps = [
                "//lib:cpp",
                "//ub_dantzig:libdantzig",
                "//ub_surrogate:libsurrogate",
                "//lb_greedy:libgreedy",
                "//lb_greedynlogn:libgreedynlogn",
        ],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
        deps = ["//lib:cpp", "//lib:cppsolver", ":libbellman"],
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
                "//test_instances:data",
        ],
        timeout = "short",
)

py_binary(
        name = "bellman_1_bench",
        srcs = ["bellman_1_bench.py"],
        main = "bellman_1_bench.py",
        data = [
                "@BenchTools//:bench",
                "@KnapsackPisingerInstances//:data",
                ":main",
        ],
)

py_binary(
        name = "bellman_1_bench_2",
        srcs = ["bellman_1_bench_2.py"],
        main = "bellman_1_bench_2.py",
        data = [
                "@BenchTools//:bench",
                "@KnapsackPisingerInstances//:data",
                ":main",
        ],
)

