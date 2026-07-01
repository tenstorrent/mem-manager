workspace(name = "mem_manager")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_googletest",
    urls = ["https://github.com/google/googletest/archive/release-1.11.0.zip"],
    sha256 = "353571c2440176ded91c2de6d6cd88ddd41401d14692ec1f99e35d013feda55a",
    strip_prefix = "googletest-release-1.11.0",
)

http_archive(
    name = "bazel_skylib",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.2.0/bazel-skylib-1.2.0.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.2.0/bazel-skylib-1.2.0.tar.gz",
    ],
    sha256 = "af87959afe497dc8dfd4c6cb66e1279cb98ccc84284619ebfec27d9c09a903de",
)

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

# Local compat shim publishing the @rules_hdl//verilog:providers.bzl surface.
# Under Bazel 6 + WORKSPACE the verilog_library aggregators build, but the
# @rules_verilator-driven Verilator targets in //test are only exercised under
# Bazel 7 + bzlmod (see MODULE.bazel and .gitlab-ci.yml).
local_repository(
    name = "rules_hdl",
    path = "bazel/rules_hdl_compat",
)

# Needed by rules_hdl_compat's load of @rules_verilog for the upstream
# VerilogInfo provider symbol.
http_archive(
    name = "rules_verilog",
    url = "https://github.com/hw-bzl/bazel_rules_verilog/releases/download/v1.1.0/bazel_rules_verilog-1.1.0.tar.gz",
    sha256 = "043196310d1ba692ec217c3778663da0d232a3746ba6291d3a12d6461de24021",
    strip_prefix = "bazel_rules_verilog-1.1.0",
)
