cc_library(
        name = "libdpreaching",
        hdrs = ["dpreaching.hpp"],
        srcs = ["dpreaching.cpp"],
        visibility = ["//visibility:public"],
        deps = ["//lib:cpp"],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
        deps = ["//lib:cpp", ":libdpreaching"],
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

