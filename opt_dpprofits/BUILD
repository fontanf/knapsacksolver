cc_library(
        name = "libdpprofits",
        hdrs = ["dpprofits.hpp"],
        srcs = ["dpprofits.cpp"],
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
        deps = [
                "//lib:cpp",
                ":libdpprofits",
                "//ub_surrogate:libsurrogate",
                "//ub_dantzig:libdantzig",
                "//lb_greedy:libgreedy",
                "//lb_ls:libls",
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

