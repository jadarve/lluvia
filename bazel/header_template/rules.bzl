"""
"""


def _cc_header_template(ctx):

    template = ctx.files.template[0]
    template_out = ctx.actions.declare_file(template.basename[:-3], sibling=template)

    executable = ctx.attr._executable

    args = ctx.actions.args()
    args.add("-D", "LUA:print('moni moni')")
    args.add("-o", template_out.path)
    args.add(template.path)

    ctx.actions.run(
        outputs = [template_out],
        inputs = [template],
        arguments = [args],
        executable = executable.files_to_run,
        mnemonic="template"
    )

    return [
        DefaultInfo(files=depset([template_out]))
    ]

cc_header_template = rule(
    implementation = _cc_header_template,
    attrs = {
        "template": attr.label(allow_single_file=[".h.in", ".hpp.in"]),
        "vars": attr.string_list(),
        "_executable": attr.label(
            default=Label("@//tools/expand_template:expand_template"),
            executable=True,
            allow_files=True,
            cfg="host")
    },
    provides = [DefaultInfo]
)
