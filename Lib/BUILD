cc_library(
        name = "cpp",
        srcs = ["Instance.cpp", "Solution.cpp", "Tools.cpp"],
        hdrs = ["Instance.hpp", "Solution.hpp", "Tools.hpp"],
        visibility = ["//visibility:public"],
)

cc_test(
        name = "cppTest",
        srcs = ["Test.cpp"],
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
