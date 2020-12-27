load("//knapsacksolver:variables.bzl", "STDCPP")

cc_library(
        name = "greedy",
        hdrs = ["greedy.hpp"],
        srcs = ["greedy.cpp"],
        deps = ["//knapsacksolver:knapsacksolver"],
        visibility = ["//visibility:public"],
)

cc_test(
        name = "greedy_test",
        srcs = ["greedy_test.cpp"],
        deps = [
                "//knapsacksolver:tester",
                ":greedy",
                ":minknap",
        ],
        timeout = "moderate",
)

################################################################################

cc_library(
        name = "greedynlogn",
        hdrs = ["greedynlogn.hpp"],
        srcs = ["greedynlogn.cpp"],
        deps = [
                "//knapsacksolver:knapsacksolver",
                ":greedy",
        ],
        visibility = ["//visibility:public"],
)

cc_test(
        name = "greedynlogn_test",
        srcs = ["greedynlogn_test.cpp"],
        deps = [
                "//knapsacksolver:tester",
                ":greedynlogn",
                ":minknap"
        ],
        timeout = "moderate",
)

################################################################################
################################################################################

cc_library(
        name = "dantzig",
        hdrs = ["dantzig.hpp"],
        srcs = ["dantzig.cpp"],
        deps = ["//knapsacksolver:knapsacksolver"],
        visibility = ["//visibility:public"],
)

################################################################################

cc_library(
        name = "dembo",
        hdrs = ["dembo.hpp"],
        srcs = ["dembo.cpp"],
        deps = ["//knapsacksolver:knapsacksolver_hpp"],
        visibility = ["//visibility:public"],
)

################################################################################

cc_library(
        name = "surrelax",
        hdrs = ["surrelax.hpp"],
        srcs = ["surrelax.cpp"],
        deps = [
                "//knapsacksolver:knapsacksolver",
                ":dantzig",
                ":minknap_hpp",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_test(
        name = "surrelax_test",
        srcs = ["surrelax_test.cpp"],
        deps = [
                "//knapsacksolver:tester",
                ":surrelax",
                ":minknap",
        ],
        copts = STDCPP,
        timeout = "moderate",
)

################################################################################
################################################################################

cc_library(
        name = "branchandbound",
        hdrs = ["branchandbound.hpp"],
        srcs = ["branchandbound.cpp"],
        deps = [
                "//knapsacksolver:knapsacksolver",
                ":dembo",
                ":dantzig",
                ":greedynlogn",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_test(
        name = "branchandbound_test",
        srcs = ["branchandbound_test.cpp"],
        deps = [
                "//knapsacksolver:tester",
                ":branchandbound",
                ":minknap",
        ],
        timeout = "moderate",
)

################################################################################

cc_library(
        name = "bellman",
        hdrs = ["bellman.hpp"],
        srcs = ["bellman.cpp"],
        deps = [
                "//knapsacksolver:knapsacksolver",
                ":dembo",
                ":dantzig",
                ":greedynlogn",
        ],
        copts = STDCPP,
        linkopts = select({
                "@bazel_tools//src/conditions:windows": [],
                "//conditions:default":                 ["-lpthread"],
        }),
        visibility = ["//visibility:public"],
)

cc_test(
        name = "bellman_test",
        srcs = ["bellman_test.cpp"],
        deps = [
                "//knapsacksolver:tester",
                ":bellman"
        ],
        timeout = "moderate",
)

################################################################################

cc_library(
        name = "dpprofits",
        hdrs = ["dpprofits.hpp"],
        srcs = ["dpprofits.cpp"],
        deps = [
                "//knapsacksolver:knapsacksolver",
                ":dembo"
        ],
        visibility = ["//visibility:public"],
)

cc_test(
        name = "dpprofits_test",
        srcs = ["dpprofits_test.cpp"],
        deps = [
                "//knapsacksolver:tester",
                ":dpprofits",
                ":bellman",
        ],
        timeout = "moderate",
)

################################################################################

cc_library(
        name = "balknap",
        hdrs = ["balknap.hpp"],
        srcs = ["balknap.cpp"],
        deps = [
                "//knapsacksolver:knapsacksolver",
                ":greedy",
                ":greedynlogn",
                ":dembo",
                ":surrelax",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_test(
        name = "balknap_test",
        srcs = ["balknap_test.cpp"],
        deps = [
                "//knapsacksolver:tester",
                ":balknap",
                ":minknap",
        ],
        timeout = "moderate",
)

################################################################################

cc_library(
        name = "expknap",
        hdrs = ["expknap.hpp"],
        srcs = ["expknap.cpp"],
        deps = [
                "//knapsacksolver:knapsacksolver",
                ":greedy",
                ":greedynlogn",
                ":dembo",
                ":surrelax",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_test(
        name = "expknap_test",
        srcs = ["expknap_test.cpp"],
        deps = [
                "//knapsacksolver:tester",
                ":expknap",
                ":minknap",
        ],
        timeout = "moderate",
)

################################################################################

cc_library(
        name = "minknap",
        hdrs = ["minknap.hpp"],
        srcs = ["minknap.cpp"],
        deps = [
                "//knapsacksolver:knapsacksolver",
                ":greedy",
                ":greedynlogn",
                ":dembo",
                ":surrelax",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_library(
        name = "minknap_hpp",
        hdrs = ["minknap.hpp"],
        srcs = [],
        deps = [],
        visibility = ["//visibility:public"],
)

cc_test(
        name = "minknap_test",
        srcs = ["minknap_test.cpp"],
        deps = [
                "//knapsacksolver:tester",
                ":minknap",
                ":bellman",
        ],
        timeout = "long",
)

################################################################################

cc_library(
        name = "algorithms",
        hdrs = ["algorithms.hpp"],
        srcs = ["algorithms.cpp"],
        deps = [
                ":greedy",
                ":greedynlogn",
                ":dembo",
                ":surrelax",
                ":branchandbound",
                ":expknap",
                ":bellman",
                ":dpprofits",
                ":balknap",
                ":minknap",
                "@boost//:program_options",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

