cc_binary(
        name = "rec",
        srcs = ["Rec.cpp"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
        deps = [
                "//Lib:cpp",
        ],
)

cc_binary(
        name = "list",
        srcs = ["List.cpp"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
        deps = [
                "//Lib:cpp",
        ],
)

cc_binary(
        name = "map",
        srcs = ["Map.cpp"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
        deps = [
                "//Lib:cpp",
        ],
)

cc_binary(
        name = "it",
        srcs = ["It.cpp"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
        deps = [
                "//Lib:cpp",
        ],
)

cc_test(
        name = "test",
        srcs = ["Test.cpp"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
        ],
        deps = [
                "//Lib:cpp",
                "@googletest//:gtest_main",
        ],
        data = [
                ":rec",
                ":map",
                ":it",
                ":list",
                "//TestInstances:data",
        ],
        timeout = "short",
)

py_binary(
        name = "mapBench",
        srcs = ["MapBench.py"],
        main = "MapBench.py",
        data = [
                "@BenchTools//:bench",
                "@KnapsackPisingerInstances//:data",
                ":map",
        ],
)

py_binary(
        name = "recBench",
        srcs = ["RecBench.py"],
        main = "RecBench.py",
        data = [
                "@BenchTools//:bench",
                "@KnapsackPisingerInstances//:data",
                ":rec",
        ],
)

py_binary(
        name = "itBench",
        srcs = ["ItBench.py"],
        main = "ItBench.py",
        data = [
                "@BenchTools//:bench",
                "@KnapsackPisingerInstances//:data",
                ":it",
        ],
)

py_binary(
        name = "listBench",
        srcs = ["ListBench.py"],
        main = "ListBench.py",
        data = [
                "@BenchTools//:bench",
                "@KnapsackPisingerInstances//:data",
                ":list",
        ],
)

py_binary(
        name = "itBench2",
        srcs = ["ItBench2.py"],
        main = "ItBench2.py",
        data = [
                "@BenchTools//:bench",
                "@KnapsackPisingerInstances//:data",
                ":it",
        ],
)

py_binary(
        name = "recBench2",
        srcs = ["RecBench2.py"],
        main = "RecBench2.py",
        data = [
                "@BenchTools//:bench",
                "@KnapsackPisingerInstances//:data",
                ":rec",
        ],
)

py_binary(
        name = "listBench2",
        srcs = ["ListBench2.py"],
        main = "ListBench2.py",
        data = [
                "@BenchTools//:bench",
                "@KnapsackPisingerInstances//:data",
                ":list",
        ],
)
