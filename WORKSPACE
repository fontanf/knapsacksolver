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
    #name = "benchtools",
    #remote = "https://github.com/fontanf/benchtools.git",
    #commit = "4dffad9d44c2e1300055a50c5a8b231ee5a8ddae",
#)

local_repository(
        name = "benchtools",
        path = "/home/fontanf/Dev/bench/",
)

local_repository(
        name = "knapsack_instances_pisinger",
        path = "/home/fontanf/Dev/knapsack_instances_pisinger/",
)
