cc_library(
        name = "libbellman",
        hdrs = ["bellman_array.hpp", "bellman_list.hpp"],
        srcs = ["bellman_array.cpp", "bellman_list.cpp"],
        visibility = ["//visibility:public"],
        deps = [
                "//lib:cpp",
                "//ub_dembo:libdembo",
        ],
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

