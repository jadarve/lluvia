"""
"""


def _expand_template(ctx):

    template = ctx.files.template[0]
    template_out = ctx.actions.declare_file(template.basename[:-3], sibling=template)

    inputs = [template]
    executable = ctx.attr._executable

    args = ctx.actions.args()
    
    for key, value in ctx.attr.vars.items():
        args.add("-D", "{0}:{1}".format(key, value))

    for key, value in ctx.attr.file_vars.items():
        
        found = False
        for fg in ctx.attr.data:
            for f in fg.files.to_list():

                # When lluvia is used as external dependency, the path to
                # files within the repo have prefix "external/lluvia".
                # Comparing the final part of the path with value enables
                # matching the same file either if it is used from within the
                # repo or as an external dependency
                if f.path.endswith(value):
                    found = True
                    inputs.append(f)
                    args.add("-F", "{0}:{1}".format(key, f.path))
                    break

        if not found:
            fail("file '{0}' not found in data".format(value), "file_vars")
    
    args.add("-o", template_out.path)
    args.add(template.path)

    ctx.actions.run(
        outputs = [template_out],
        inputs = inputs,
        arguments = [args],
        executable = executable.files_to_run,
        mnemonic="template"
    )

    return [
        DefaultInfo(files=depset([template_out]))
    ]

expand_template = rule(
    implementation = _expand_template,
    attrs = {
        "template": attr.label(allow_single_file=[".in", ".in"]),
        "vars": attr.string_dict(),
        "file_vars": attr.string_dict(),
        "data": attr.label_list(allow_files=True),
        "_executable": attr.label(
            default=Label("@lluvia//lluvia/tools/expand_template:expand_template"),
            executable=True,
            allow_files=True,
            cfg="host")
    },
    provides = [DefaultInfo]
)
