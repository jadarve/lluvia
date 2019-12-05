"""
"""

def _glsl_shader_impl(ctx):

    shader = ctx.file.shader

    spirv_name = shader.basename[:-len(shader.extension)] + "spv"

    spirv = ctx.actions.declare_file(spirv_name, sibling=shader)
    cmd = "glslc -o %s %s" % (spirv.path, shader.path)
    
    ctx.actions.run_shell(
        inputs = [shader],
        outputs = [spirv],
        command = cmd,
        progress_message = "compiling GLSL",
        mnemonic = 'GLSLCompile'
    )

    return [DefaultInfo(
        files = depset([spirv])
    )]


glsl_shader = rule(
    implementation = _glsl_shader_impl,
    attrs = {
        "shader": attr.label(allow_single_file=[".comp"]),
        "includes": attr.string_list(allow_empty=True),
    },
    provides = [DefaultInfo]
)
