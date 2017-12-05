new_git_repository(
    name = "googletest",
    build_file_content = """
cc_library(
        name = "gtest",
        srcs = ["googletest/src/gtest-all.cc", "googlemock/src/gmock-all.cc",],
        hdrs = glob(["**/*.h", "googletest/src/*.cc", "googlemock/src/*.cc",]),
        includes = ["googlemock", "googletest", "googletest/include", "googlemock/include",],
        linkopts = ["-pthread"],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "gtest_main",
        srcs = ["googlemock/src/gmock_main.cc"],
        linkopts = ["-pthread"],
        visibility = ["//visibility:public"],
        deps = [":gtest"],
)
""",
    remote = "https://github.com/google/googletest",
    tag = "release-1.8.0",
)

#git_repository(
    #name = "BenchTools",
    #remote = "https://github.com/fontanf/BenchTools.git",
    #commit = "778f79a1e2c1da8ab4ece821eaae00f6b7a815e5"
#)

local_repository(
        name = "BenchTools",
        path = "/home/fontanf/Dev/BenchTools/",
)

local_repository(
        name = "KnapsackPisingerInstances",
        path = "/home/fontanf/Dev/KnapsackPisingerInstances/",
)
