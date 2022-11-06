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
                ":dynamic_programming_primal_dual",
        ],
        timeout = "moderate",
)

################################################################################

cc_library(
        name = "greedy_nlogn",
        hdrs = ["greedy_nlogn.hpp"],
        srcs = ["greedy_nlogn.cpp"],
        deps = [
                "//knapsacksolver:knapsacksolver",
                ":greedy",
        ],
        visibility = ["//visibility:public"],
)

cc_test(
        name = "greedy_nlogn_test",
        srcs = ["greedy_nlogn_test.cpp"],
        deps = [
                "//knapsacksolver:tester",
                ":greedy_nlogn",
                ":dynamic_programming_primal_dual"
        ],
        timeout = "moderate",
)

################################################################################
################################################################################

cc_library(
        name = "upper_bound_dantzig",
        hdrs = ["upper_bound_dantzig.hpp"],
        srcs = ["upper_bound_dantzig.cpp"],
        deps = ["//knapsacksolver:knapsacksolver"],
        visibility = ["//visibility:public"],
)

################################################################################

cc_library(
        name = "upper_bound_dembo",
        hdrs = ["upper_bound_dembo.hpp"],
        srcs = ["upper_bound_dembo.cpp"],
        deps = ["//knapsacksolver:knapsacksolver_hpp"],
        visibility = ["//visibility:public"],
)

################################################################################

cc_library(
        name = "surrogate_relaxation",
        hdrs = ["surrogate_relaxation.hpp"],
        srcs = ["surrogate_relaxation.cpp"],
        deps = [
                "//knapsacksolver:knapsacksolver",
                ":upper_bound_dantzig",
                ":dynamic_programming_primal_dual_hpp",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_test(
        name = "surrogate_relaxation_test",
        srcs = ["surrogate_relaxation_test.cpp"],
        deps = [
                "//knapsacksolver:tester",
                ":surrogate_relaxation",
                ":dynamic_programming_primal_dual",
        ],
        copts = STDCPP,
        timeout = "moderate",
)

################################################################################
################################################################################

cc_library(
        name = "branch_and_bound",
        hdrs = ["branch_and_bound.hpp"],
        srcs = ["branch_and_bound.cpp"],
        deps = [
                "//knapsacksolver:knapsacksolver",
                ":upper_bound_dembo",
                ":upper_bound_dantzig",
                ":greedy_nlogn",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_test(
        name = "branch_and_bound_test",
        srcs = ["branch_and_bound_test.cpp"],
        deps = [
                "//knapsacksolver:tester",
                ":branch_and_bound",
                ":dynamic_programming_primal_dual",
        ],
        timeout = "moderate",
)

################################################################################

cc_library(
        name = "dynamic_programming_bellman",
        hdrs = ["dynamic_programming_bellman.hpp"],
        srcs = ["dynamic_programming_bellman.cpp"],
        deps = [
                "//knapsacksolver:knapsacksolver",
                ":upper_bound_dembo",
                ":upper_bound_dantzig",
                ":greedy_nlogn",
        ],
        copts = STDCPP,
        linkopts = select({
                "@bazel_tools//src/conditions:windows": [],
                "//conditions:default":                 ["-lpthread"],
        }),
        visibility = ["//visibility:public"],
)

cc_test(
        name = "dynamic_programming_bellman_test",
        srcs = ["dynamic_programming_bellman_test.cpp"],
        deps = [
                "//knapsacksolver:tester",
                ":dynamic_programming_bellman"
        ],
        timeout = "moderate",
)

################################################################################

cc_library(
        name = "dynamic_programming_profits",
        hdrs = ["dynamic_programming_profits.hpp"],
        srcs = ["dynamic_programming_profits.cpp"],
        deps = [
                "//knapsacksolver:knapsacksolver",
                ":upper_bound_dembo"
        ],
        visibility = ["//visibility:public"],
)

cc_test(
        name = "dynamic_programming_profits_test",
        srcs = ["dynamic_programming_profits_test.cpp"],
        deps = [
                "//knapsacksolver:tester",
                ":dynamic_programming_profits",
                ":dynamic_programming_bellman",
        ],
        timeout = "moderate",
)

################################################################################

cc_library(
        name = "dynamic_programming_balancing",
        hdrs = ["dynamic_programming_balancing.hpp"],
        srcs = ["dynamic_programming_balancing.cpp"],
        deps = [
                "//knapsacksolver:knapsacksolver",
                ":greedy",
                ":greedy_nlogn",
                ":upper_bound_dembo",
                ":surrogate_relaxation",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_test(
        name = "dynamic_programming_balancing_test",
        srcs = ["dynamic_programming_balancing_test.cpp"],
        deps = [
                "//knapsacksolver:tester",
                ":dynamic_programming_balancing",
                ":dynamic_programming_primal_dual",
        ],
        timeout = "moderate",
)

################################################################################

cc_library(
        name = "branch_and_bound_primal_dual",
        hdrs = ["branch_and_bound_primal_dual.hpp"],
        srcs = ["branch_and_bound_primal_dual.cpp"],
        deps = [
                "//knapsacksolver:knapsacksolver",
                ":greedy",
                ":greedy_nlogn",
                ":upper_bound_dembo",
                ":surrogate_relaxation",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_test(
        name = "branch_and_bound_primal_dual_test",
        srcs = ["branch_and_bound_primal_dual_test.cpp"],
        deps = [
                "//knapsacksolver:tester",
                ":branch_and_bound_primal_dual",
                ":dynamic_programming_primal_dual",
        ],
        timeout = "moderate",
)

################################################################################

cc_library(
        name = "dynamic_programming_primal_dual",
        hdrs = ["dynamic_programming_primal_dual.hpp"],
        srcs = ["dynamic_programming_primal_dual.cpp"],
        deps = [
                "//knapsacksolver:knapsacksolver",
                ":greedy",
                ":greedy_nlogn",
                ":upper_bound_dembo",
                ":surrogate_relaxation",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

cc_library(
        name = "dynamic_programming_primal_dual_hpp",
        hdrs = ["dynamic_programming_primal_dual.hpp"],
        srcs = [],
        deps = [],
        visibility = ["//visibility:public"],
)

cc_test(
        name = "dynamic_programming_primal_dual_test",
        srcs = ["dynamic_programming_primal_dual_test.cpp"],
        deps = [
                "//knapsacksolver:tester",
                ":dynamic_programming_primal_dual",
                ":dynamic_programming_bellman",
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
                ":greedy_nlogn",
                ":upper_bound_dembo",
                ":surrogate_relaxation",
                ":branch_and_bound",
                ":branch_and_bound_primal_dual",
                ":dynamic_programming_bellman",
                ":dynamic_programming_profits",
                ":dynamic_programming_balancing",
                ":dynamic_programming_primal_dual",
                "@boost//:program_options",
        ],
        copts = STDCPP,
        visibility = ["//visibility:public"],
)

