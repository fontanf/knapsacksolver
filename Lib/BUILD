cc_library(
        name = "cpp",
        srcs = ["Parser.cpp", "PartialInstance.cpp"],
        hdrs = ["Parser.hpp", "PartialInstance.hpp"],
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
