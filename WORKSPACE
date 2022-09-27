load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
  name = "com_google_googletest",
  urls = ["https://github.com/google/googletest/archive/release-1.11.0.zip"],
  sha256 = "353571c2440176ded91c2de6d6cd88ddd41401d14692ec1f99e35d013feda55a",
  strip_prefix = "googletest-release-1.11.0",
)

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
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

rules_hdl_hash="dbca9e91f08d7e32f22279783916874077b64e36"
http_archive(
  name = "rules_hdl",
  sha256 = "775ae2ee907f8101ad183577a225a984cd25a4dd03e6826f0df509d4902db392",
  strip_prefix = "bazel_rules_hdl-{commit}".format(commit=rules_hdl_hash),
  url = "https://aus-gitlab.local.tenstorrent.com/vkoyyalagunta/bazel_rules_hdl/-/archive/{commit}/bazel_rules_hdl-{commit}.tar.bz2".format(commit=rules_hdl_hash),
)

rules_verilator_hash="49b7e16f32c14698e35da2214eaf5ea1ebe86bc1"
http_archive(
  name = "rules_verilator",
  sha256 = "307c5b6c27822f654a5a8dbb2d5271a5f0ac727bd86b49565d2c630a4b20f6c2",
  strip_prefix = "rules_verilator-{commit}".format(commit=rules_verilator_hash),
  url = "https://aus-gitlab.local.tenstorrent.com/vkoyyalagunta/rules_verilator/-/archive/{commit}/rules_verilator-{commit}.tar.bz2".format(commit=rules_verilator_hash),
)

load(
  "@rules_verilator//verilator:repositories.bzl",
  "rules_verilator_dependencies",
  "rules_verilator_toolchains"
)
rules_verilator_dependencies()
rules_verilator_toolchains()

# Register toolchain dependencies
load("@rules_m4//m4:m4.bzl", "m4_register_toolchains")
m4_register_toolchains()

load("@rules_flex//flex:flex.bzl", "flex_register_toolchains")
flex_register_toolchains()

load("@rules_bison//bison:bison.bzl", "bison_register_toolchains")
bison_register_toolchains()
