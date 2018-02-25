cc_library(
        name = "libbabprimaldual",
        hdrs = ["babprimaldual.hpp"],
        srcs = ["babprimaldual.cpp"],
        visibility = ["//visibility:public"],
        deps = [
                "//lib:cpp",
                "//ub_dantzig:libdantzig",
                "//ub_surrogate:libsurrogate",
                "//lb_greedy:libgreedy",
        ],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [":libbabprimaldual", "//lib:cpp"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
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

