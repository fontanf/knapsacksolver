cc_library(
        name = "cpp",
        srcs = ["Parser.cpp", "PartialInstance.cpp", "Tools.cpp"],
        hdrs = ["Parser.hpp", "PartialInstance.hpp", "Tools.hpp"],
        visibility = ["//visibility:public"],
)

cc_test(
        name = "cppTest",
        srcs = ["ParserTest.cpp"],
        deps = [
                ":cpp", 
                "@googletest//:gtest_main",
                "@system_libs//:boost",
        ],
        data = [
                "//KnapsackFontan/TestInstances:data",
        ],
        timeout = "short",
)
