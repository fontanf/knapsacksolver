cc_library(
        name = "libdpprofits",
        hdrs = ["dpprofits.hpp"],
        srcs = [
                "dpprofits_array.cpp",
                "dpprofits_list.cpp"
        ],
        deps = [
                "//lib:cpp",
                "//ub_dembo:libdembo",
        ],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/opt_dpprofits/",
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [":libdpprofits"],
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

