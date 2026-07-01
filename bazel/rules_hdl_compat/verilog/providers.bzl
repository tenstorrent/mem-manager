# Compatibility shim providing the `@rules_hdl//verilog:providers.bzl` surface
# downstream Tenstorrent repos still load. Each verilog_library target emits
# two providers:
#   - The legacy VerilogInfo defined here (dag/plis), for downstream callers.
#   - The upstream @rules_verilog VerilogInfo, so @rules_verilator can consume
#     cvm's verilog_library aggregators directly via the `module = ...` attr.
#
# Delete this directory (along with the @rules_hdl wiring in MODULE.bazel /
# WORKSPACE) once downstream consumers migrate to @rules_verilog.

load("@rules_verilog//verilog:defs.bzl", _UpstreamVerilogInfo = "VerilogInfo")

VerilogInfo = provider(
    doc = "Legacy DAG-based VerilogInfo. Provider identity must stay stable for downstream compatibility.",
    fields = {
        "dag": "depset of DAG entries (struct of srcs/hdrs/libs/fs/views/deps/label/strip_include_prefix).",
        "plis": "depset (unused today; kept for downstream field compatibility).",
    },
)

def make_dag_entry(srcs, hdrs, libs, fs, views, deps, label, strip_include_prefix):
    return struct(
        srcs = tuple(srcs),
        hdrs = tuple(hdrs),
        libs = tuple(libs),
        fs = tuple(fs),
        views = tuple(views),
        deps = tuple(deps),
        label = label,
        strip_include_prefix = strip_include_prefix,
    )

def make_verilog_info(new_entries = (), old_infos = ()):
    return VerilogInfo(
        dag = depset(
            direct = new_entries,
            order = "postorder",
            transitive = [x.dag for x in old_infos],
        ),
        plis = depset(),
    )

def _verilog_library_impl(ctx):
    legacy = make_verilog_info(
        new_entries = [make_dag_entry(
            srcs = ctx.files.srcs,
            hdrs = ctx.files.hdrs,
            libs = ctx.files.libs,
            fs = ctx.files.fs,
            views = ctx.attr.views,
            deps = ctx.attr.deps,
            label = ctx.label,
            strip_include_prefix = ctx.attr.strip_include_prefix,
        )],
        old_infos = [dep[VerilogInfo] for dep in ctx.attr.deps],
    )

    # Flatten the legacy dag into upstream's view.
    #
    # hw-bzl `rules_verilator` (see @rules_verilator//verilator/private/common.bzl)
    # treats `srcs`, `hdrs`, and `data` from upstream `VerilogInfo` symmetrically:
    # `collect_verilog_inputs` concatenates them into `all_files`, then
    # `add_verilator_compile_args` passes the whole list to Verilator via
    # `args.add_all(verilog_files, map_each = only_sv)`. `only_sv` filters by
    # file extension (`.sv`/`.v` only), so `.svh`/`.vh` files become action
    # inputs (sandbox-resident, reachable by `\`include`) without appearing on
    # the Verilator command line — exactly the "header" semantics we want.
    #
    # Putting our shim's `hdrs` into upstream `srcs` plus exposing each hdr's
    # directory via upstream `includes` (so `-I<dir>` flags are emitted)
    # therefore behaves correctly *iff* the caller honors the file-extension
    # convention (`.svh`/`.vh` for include-fragments). Callers that try to
    # smuggle a `.sv` include-fragment through `hdrs` (e.g. pmu_fragment_gen
    # naming its `always_comb`-block snippets `gen_*_monitor.sv`) still get
    # compiled standalone by Verilator — that's a caller-side bug; the shim
    # can't fix it without an extension-aware filter that diverges from
    # upstream semantics.
    all_srcs = []
    all_hdrs = []
    include_dirs = {}
    for entry in legacy.dag.to_list():
        all_srcs += list(entry.srcs)
        for hdr in entry.hdrs:
            all_hdrs.append(hdr)
            # Default include search path is the file's directory. If the
            # entry declared `strip_include_prefix`, expose the parent path
            # without that prefix instead — matches the legacy @rules_hdl
            # behavior so an `\`include "foo/bar.svh"` line resolves to a
            # file laid out at `<prefix>/foo/bar.svh`.
            dirname = hdr.dirname
            prefix = entry.strip_include_prefix
            if prefix:
                # `strip_include_prefix` is a path relative to the package
                # (or absolute starting with "/"). Find where it appears in
                # the file's dirname and strip it.
                marker = prefix.lstrip("/")
                idx = dirname.find("/" + marker)
                if idx >= 0:
                    dirname = dirname[:idx]
                elif dirname.endswith(marker):
                    dirname = dirname[:-len(marker)].rstrip("/")
            include_dirs[dirname] = None

    upstream = _UpstreamVerilogInfo(
        srcs = depset(all_srcs + all_hdrs),
        hdrs = depset(),
        includes = depset(include_dirs.keys()),
        data = depset(),
        standard = "",
        top_module = ctx.attr.top_module,
        deps = depset(),
    )

    return [legacy, upstream, DefaultInfo(files = depset(all_srcs + all_hdrs))]

verilog_library = rule(
    attrs = {
        "srcs": attr.label_list(allow_files = True),
        # `hdrs` is restricted to header-style extensions because hw-bzl
        # rules_verilator's CLI filter (`only_sv`) drops only `.svh`/`.vh` from
        # the Verilator command line. Any `.sv`/`.v` smuggled through `hdrs`
        # would be compiled as a top-level translation unit by Verilator,
        # which is exactly what the @rules_hdl `hdrs` semantic is supposed to
        # avoid. Reject those file extensions here so the caller gets a clear
        # error pointing at the offending rule (rename to .svh, or move to
        # srcs if it's a real source).
        "hdrs": attr.label_list(allow_files = [".svh", ".vh"]),
        "libs": attr.label_list(allow_files = True),
        "fs": attr.label_list(allow_files = True),
        "views": attr.string_list(),
        "deps": attr.label_list(providers = [VerilogInfo]),
        "strip_include_prefix": attr.string(),
        "top_module": attr.string(
            default = "",
            doc = "Top-module name. Surfaced on the upstream VerilogInfo so verilator_cc_library can find it.",
        ),
    },
    implementation = _verilog_library_impl,
)
