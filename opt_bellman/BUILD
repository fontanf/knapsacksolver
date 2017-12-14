cc_library(
        name = "libbellman",
        hdrs = ["bellman.hpp"],
        srcs = ["bellman.cpp"],
        visibility = ["//visibility:public"],
        deps = ["//lib:cpp", ":libit"],
)

cc_binary(
        name = "bellman_1",
        srcs = ["bellman_1_main.cpp"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
        deps = ["//lib:cpp", ":libbellman"],
)

cc_binary(
        name = "bellman_2",
        srcs = ["bellman_2_main.cpp"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
        deps = ["//lib:cpp", ":libbellman"],
)

cc_library(
        name = "libit",
        srcs = ["it.cpp"],
        hdrs = ["it.hpp"],
        visibility = ["//visibility:public"],
        deps = ["//lib:cpp"],
)

cc_library(
        name = "cpp",
        srcs = ["instance.cpp", "solution.cpp"],
        hdrs = ["instance.hpp", "solution.hpp"],
        visibility = ["//visibility:public"],
)

cc_binary(
        name = "rec",
        srcs = ["rec.cpp"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
        deps = ["//lib:cpp"],
)

cc_binary(
        name = "stack",
        srcs = ["stack.cpp"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
        deps = ["//lib:cpp"],
)

cc_binary(
        name = "map",
        srcs = ["map.cpp"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
        deps = ["//lib:cpp"],
)

cc_binary(
        name = "it",
        srcs = ["it_main.cpp"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
        deps = ["//lib:cpp", ":libit"],
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
                ":bellman_1",
                ":bellman_2",
                ":rec",
                ":map",
                ":it",
                ":stack",
                "//test_instances:data",
        ],
        timeout = "short",
)

py_binary(
        name = "map_bench",
        srcs = ["map_bench.py"],
        main = "map_bench.py",
        data = [
                "@BenchTools//:bench",
                "@KnapsackPisingerInstances//:data",
                ":map",
        ],
)

py_binary(
        name = "rec_bench",
        srcs = ["rec_bench.py"],
        main = "rec_bench.py",
        data = [
                "@BenchTools//:bench",
                "@KnapsackPisingerInstances//:data",
                ":rec",
        ],
)

py_binary(
        name = "it_bench",
        srcs = ["it_bench.py"],
        main = "it_bench.py",
        data = [
                "@BenchTools//:bench",
                "@KnapsackPisingerInstances//:data",
                ":it",
        ],
)

py_binary(
        name = "stack_bench",
        srcs = ["stack_bench.py"],
        main = "stack_bench.py",
        data = [
                "@BenchTools//:bench",
                "@KnapsackPisingerInstances//:data",
                ":stack",
        ],
)

py_binary(
        name = "it_bench2",
        srcs = ["it_bench2.py"],
        main = "it_bench2.py",
        data = [
                "@BenchTools//:bench",
                "@KnapsackPisingerInstances//:data",
                ":it",
        ],
)

py_binary(
        name = "rec_bench2",
        srcs = ["rec_bench2.py"],
        main = "rec_bench2.py",
        data = [
                "@BenchTools//:bench",
                "@KnapsackPisingerInstances//:data",
                ":rec",
        ],
)

py_binary(
        name = "stack_bench2",
        srcs = ["stack_bench2.py"],
        main = "stack_bench2.py",
        data = [
                "@BenchTools//:bench",
                "@KnapsackPisingerInstances//:data",
                ":stack",
        ],
)
