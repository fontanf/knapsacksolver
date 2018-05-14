cc_library(
        name = "libdantzig",
        hdrs = ["dantzig.hpp"],
        srcs = ["dantzig.cpp"],
        visibility = ["//visibility:public"],
        deps = ["//lib:cpp"],
)

cc_library(
        name = "libdantzigh",
        hdrs = ["dantzig.hpp"],
        visibility = ["//visibility:public"],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [":libdantzig"],
)
