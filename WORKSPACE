load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

git_repository(
    name = "googletest",
    remote = "https://github.com/google/googletest.git",
    commit = "58d77fa8070e8cec2dc1ed015d66b454c8d78850",
    shallow_since = "1656350095 -0400",
)

git_repository(
    name = "com_github_nelhage_rules_boost",
    remote = "https://github.com/nelhage/rules_boost",
    commit = "e83dfef18d91a3e35c8eac9b9aeb1444473c0efd",
    shallow_since = "1671181466 +0000",
)
load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

http_archive(
    name = "json",
    build_file_content = """
cc_library(
        name = "json",
        hdrs = ["single_include/nlohmann/json.hpp"],
        visibility = ["//visibility:public"],
        strip_include_prefix = "single_include/"
)
""",
    urls = ["https://github.com/nlohmann/json/releases/download/v3.7.3/include.zip"],
    sha256 = "87b5884741427220d3a33df1363ae0e8b898099fbc59f1c451113f6732891014",
)

git_repository(
    name = "optimizationtools",
    remote = "https://github.com/fontanf/optimizationtools.git",
    commit = "49a279e7df70b21f8c62cb89ec58e363c053ead8",
)

local_repository(
    name = "optimizationtools_",
    path = "../optimizationtools/",
)

http_archive(
  name = "pybind11_bazel",
  strip_prefix = "pybind11_bazel-2e5f4a6beece3b92d2f87744f305eb52b6852aa9",
  urls = ["https://github.com/pybind/pybind11_bazel/archive/2e5f4a6beece3b92d2f87744f305eb52b6852aa9.zip"],
  sha256 = "9c2919136d7c3b4a4c4a4c0ac5eb97c101c5f59c417e4f1abe70c370b1590b73",
)
http_archive(
  name = "pybind11",
  build_file = "@pybind11_bazel//:pybind11.BUILD",
  strip_prefix = "pybind11-2.11.1",
  urls = ["https://github.com/pybind/pybind11/archive/v2.11.1.tar.gz"],
  sha256 = "d475978da0cdc2d43b73f30910786759d593a9d8ee05b1b6846d1eb16c6d2e0c",
)
load("@pybind11_bazel//:python_configure.bzl", "python_configure")
python_configure(name = "local_config_python")
