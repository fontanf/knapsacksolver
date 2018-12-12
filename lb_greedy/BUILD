cc_library(
        name = "libgreedy",
        hdrs = ["greedy.hpp"],
        srcs = ["greedy.cpp"],
        deps = ["//lib:cpp"],
        visibility = ["//visibility:public"],
        include_prefix = "knapsack/lb_greedy/",
)

cc_test(
        name = "test",
        srcs = ["test.cpp"],
        deps = ["//lib:tester", "//lb_greedy:libgreedy", "//opt_minknap:libminknap"],
        timeout = "moderate",
)

