cc_library(
        name = "libbalknap",
        hdrs = ["balknap.hpp"],
        srcs = ["balknap.cpp"],
        visibility = ["//visibility:public"],
        deps = [
                "//lib:cpp",
                "//lb_ls:libls",
                "//lb_greedy:libgreedy",
                "//ub_surrogate:libsurrogate",
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
        deps = [
                "//lib:cpp",
                ":libbalknap"
        ],
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
