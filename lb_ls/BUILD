cc_library(
        name = "libls",
        hdrs = ["ls.hpp"],
        srcs = ["ls.cpp"],
        visibility = ["//visibility:public"],
        deps = ["//lib:cpp"],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [":libls", "//lib:cpp"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options"
        ],
)
