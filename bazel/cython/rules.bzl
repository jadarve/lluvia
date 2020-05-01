
"""
"""

def _cy_compile(ctx):

    pyx_file = ctx.file.pyx

    args = ctx.actions.args()
    for directive in ctx.attr.directives:
        args.add(directive)
    
    cpp_translation = ctx.actions.declare_file(pyx_file.basename[:-4] + ".cpp", sibling=pyx_file)

    args.add("--output-file", cpp_translation.path)
    args.add(pyx_file.path)

    ctx.actions.run(
        inputs = [pyx_file] + ctx.files.headers,
        outputs = [cpp_translation],
        arguments = [args],
        executable = ctx.attr._cython_binary.files_to_run,
        progress_message = "Cython {0}".format(pyx_file.path),
        mnemonic = "Cython"
    )

    return [
        DefaultInfo(files=depset([cpp_translation]))
    ]


cy_compile = rule(
    implementation = _cy_compile,
    attrs = {
        "pyx": attr.label(allow_single_file=[".pyx"]),
        "headers": attr.label_list(allow_files=[".pxd", ".py"]),
        "directives": attr.string_list(default=[]),
        "strip_include_prefix": attr.string(default=""),
        "_cython_binary": attr.label(
            default=Label("@cython//:cython_binary"),
            executable=True,
            allow_files=True,
            cfg="host"
        )
    },
    provides = [DefaultInfo]
)
