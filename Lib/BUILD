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
                "@boost//:filesystem",
                "@boost//:system",
                "@boost//:program_options",
        ],
        data = [
                "//TestInstances:data",
        ],
        timeout = "short",
)
