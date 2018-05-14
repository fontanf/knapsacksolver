cc_library(
        name = "libdpprofits",
        hdrs = ["dpprofits_array.hpp", "dpprofits_list.hpp"],
        srcs = ["dpprofits_array.cpp", "dpprofits_list.cpp"],
        visibility = ["//visibility:public"],
        deps = [
                "//lib:cpp",
                "//ub_dembo:libdembo",
        ],
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

