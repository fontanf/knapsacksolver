cc_library(
        name = "cpp",
        hdrs = ["instance.hpp", "solution.hpp"],
        srcs = ["instance.cpp", "solution.cpp"],
        visibility = ["//visibility:public"],
        deps = ["//ub_dantzig:libdantzig"],
)

cc_library(
        name = "hpp",
        hdrs = ["instance.hpp", "solution.hpp"],
        visibility = ["//visibility:public"],
)

cc_test(
        name = "cpp_test",
        srcs = ["test.cpp"],
        deps = [
                ":cpp", 
                "@googletest//:gtest_main",
        ],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
        data = [
                "//TestInstances:data",
        ],
        timeout = "short",
)
