"""
"""

GlslInfo = provider(fields=[
    "includes",
    "headers"
])

# glslc -I lib/ -o moni.spv imageToBuffer.comp
def _glsl_header_library(ctx):

    print(dir(ctx))
    print(ctx.bin_dir.path)
    
    # print(ctx.build_file_path)
    # for hdr in ctx.files.hdrs:
    #     print(hdr.path)
    #     print(hdr.short_path)
    #     print(hdr.dirname)
    #     print(hdr.basename)
    #     print(dir(hdr))

    return [
        GlslInfo(
            headers=depset(direct=ctx.files.hdrs),
            includes=depset(direct=["/home/jadarve/git/lluvia/glsl/lib"]),
        )
    ]


def _glsl_shader(ctx):


    shader = ctx.file.shader

    spirv_name = shader.basename[:-len(shader.extension)] + "spv"

    spirv = ctx.actions.declare_file(spirv_name, sibling=shader)

    args = ctx.actions.args()

    for dep in ctx.attr.deps:
        for inc in dep[GlslInfo].includes.to_list():
            args.add("-I", inc)
    
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
        "deps": attr.label_list(providers=[GlslInfo]),
    },
    provides = [DefaultInfo]
)


glsl_header_library = rule(
    implementation = _glsl_header_library,
    attrs = {
        "hdrs": attr.label_list(allow_files=[".glsl"])
    }
)
