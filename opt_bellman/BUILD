cc_library(
        name = "libbellman",
        hdrs = ["bellman.hpp"],
        srcs = [
                "bellman_array.cpp",
                "bellman_list.cpp"
        ],
        deps = [
                "//lib:cpp",
                "//ub_dembo:libdembo",
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/opt_bellman/",
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [":libbellman"],
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = ["//lib:tester"],
        data = [
                ":main",
                "//data_tests:data",
        ],
        timeout = "short",
)

