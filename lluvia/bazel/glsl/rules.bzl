"""
"""

GlslInfo = provider(fields=[
    "includes",
    "headers"
])

# glslc -I lib/ -o moni.spv imageToBuffer.comp
def _glsl_header_library(ctx):

    strip_include_prefix = ctx.attr.strip_include_prefix

    includes = dict()

    # Add each header file directory to the includes
    if len(strip_include_prefix) == 0:
        for hdr in ctx.files.hdrs:
            includes[hdr.dirname] = True

    else:
        for hdr in ctx.files.hdrs:

            # check if the header is being included from the lluvia repo
            # or externally (e.g., mediapipe). For external inclusion, bazel
            # adds a "external/lluvia" path to all files of this repo, so I
            # need to include that path as part of the strip_include_prefix

            prefix_index = hdr.dirname.rfind(strip_include_prefix)
            if prefix_index >= 0:
                strip_include_prefix_full = hdr.dirname[:prefix_index] + strip_include_prefix
                includes[strip_include_prefix_full] = True

            else:
                fail("strip_include_prefix \"{0}\" not found in header \"{1}\" path.".format(
                    strip_include_prefix, hdr.path))

    return [
        GlslInfo(
            headers=depset(direct=ctx.files.hdrs),
            includes=depset(direct=includes.keys())
        )
    ]


def _glsl_shader(ctx):


    shader = ctx.file.shader

    spirv_name = shader.basename[:-len(shader.extension)] + "spv"

    spirv = ctx.actions.declare_file(spirv_name, sibling=shader)

    args = ctx.actions.args()
    inputs = [shader]

    for dep in ctx.attr.deps:

        for inc in dep[GlslInfo].includes.to_list():
            args.add("-I", inc)

        # add each header as an input file for the command.
        # This mounts the header into the sandbox running glslc and makes it
        # accessible to the compiler
        for hdr in dep[GlslInfo].headers.to_list():
            inputs.append(hdr)


    args.add("-o", spirv)
    args.add(shader.path)

    isWindows = select(
        {
            "@lluvia//lluvia:windows": True,
            "//conditions:default" : False
        }
    )

    if isWindows:
        ctx.actions.run(
            inputs = inputs,
            outputs = [spirv],
            arguments = [args],
            executable = "C:/VulkanSDK/1.2.176.1/Bin/glslc",
            progress_message = "compiling GLSL",
            mnemonic = 'GLSLC'
        )
    else:
        ctx.actions.run_shell(
            inputs = inputs,
            outputs = [spirv],
            arguments = [args],
            command = "glslc $@",
            progress_message = "compiling GLSL",
            mnemonic = 'GLSLC'
        )

    runfiles = ctx.runfiles(
        files = [spirv]
    )

    default_files = depset(direct=[spirv])

    return [
        DefaultInfo(files=default_files, runfiles=runfiles)
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
        "hdrs": attr.label_list(allow_files=[".glsl"]),
        "strip_include_prefix": attr.string(default=""),
    },
    provides = [DefaultInfo, GlslInfo]
)
