"""
"""

def _glsl_header_library(ctx):

    print(dir(ctx))

    for hdr in ctx.files.hdrs:
        print(hdr.path)
        print(hdr.short_path)
        print(hdr.dirname)
        print(hdr.basename)
        print(dir(hdr))


def _glsl_shader(ctx):

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
    implementation = _glsl_shader,
    attrs = {
        "shader": attr.label(allow_single_file=[".comp"]),
        "includes": attr.string_list(allow_empty=True),
    },
    provides = [DefaultInfo]
)


glsl_header_library = rule(
    implementation = _glsl_header_library,
    attrs = {
        "hdrs": attr.label_list(allow_files=[".glsl"])
    }
)
