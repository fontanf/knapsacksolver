cc_library(
        name = "libbalknap",
        hdrs = ["balknap.hpp"],
        srcs = ["balknap_array.cpp", "balknap_list.cpp"],
        visibility = ["//visibility:public"],
        deps = [
                "//lib:cpp",
                "//lb_greedy:libgreedy",
                "//lb_greedynlogn:libgreedynlogn",
                "//ub_dembo:libdembo",
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
                ":libbalknap",
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
