"""
"""

def _glsl_shader_impl(ctx):

    shader = ctx.file.shader

    spirv_name = shader.basename[:-len(shader.extension)] + "spv"

    spirv = ctx.actions.declare_file(spirv_name, sibling=shader)

    args = ctx.actions.args()
    args.add("-I", "glsl/include")
    args.add("-o", spirv)
    args.add(shader.path)

    # cmd = "glslc -o %s %s" % (spirv.path, shader.path)
    cmd = "glslc $@"
    
    ctx.actions.run_shell(
        inputs = [shader],
        outputs = [spirv],
        arguments = [args],
        command = cmd,
        progress_message = "compiling GLSL",
        mnemonic = 'GLSLC'
    )

    runfiles = ctx.runfiles(
        files = [spirv]
    )

    return [
        DefaultInfo(runfiles=runfiles)
    ]


glsl_shader = rule(
    implementation = _glsl_shader_impl,
    attrs = {
        "shader": attr.label(allow_single_file=[".comp"]),
        "includes": attr.string_list(allow_empty=True),
    },
    provides = [DefaultInfo]
)
