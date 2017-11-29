cc_binary(
        name = "rec",
        srcs = ["Rec.cpp"],
        deps = [
                "//Lib:cpp",
                "@boost//:filesystem",
                "@boost//:system",
                "@boost//:program_options",
        ],
)

cc_binary(
        name = "list",
        srcs = ["List.cpp"],
        deps = [
                "//Lib:cpp",
                "@boost//:filesystem",
                "@boost//:system",
                "@boost//:program_options",
        ],
)

cc_binary(
        name = "map",
        srcs = ["Map.cpp"],
        deps = [
                "//Lib:cpp",
                "@boost//:filesystem",
                "@boost//:system",
                "@boost//:program_options",
        ],
)

cc_binary(
        name = "it",
        srcs = ["It.cpp"],
        deps = [
                "//Lib:cpp",
                "@boost//:filesystem",
                "@boost//:system",
                "@boost//:program_options",
        ],
)

cc_test(
        name = "test",
        srcs = ["Test.cpp"],
        deps = [
                "//Lib:cpp",
                "@googletest//:gtest_main",
                "@boost//:filesystem",
                "@boost//:system",
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
