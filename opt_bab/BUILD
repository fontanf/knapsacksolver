cc_library(
        name = "libbab",
        hdrs = ["bab.hpp"],
        srcs = ["bab.cpp"],
        visibility = ["//visibility:public"],
        deps = [
                "//lib:cpp",
                "//ub_dembo:libdembo",
        ],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [":libbab"],
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

