#ifndef LLUVIA_CORE_GLSL_
#define LLUVIA_CORE_GLSL_

// Compute shader global invocation coordinates
#define LL_GLOBAL_COORDS_1D gl_GlobalInvocationID.x
#define LL_GLOBAL_COORDS_2D ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y)
#define LL_GLOBAL_COORDS_3D ivec3(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, gl_GlobalInvocationID.z)

// Index for the next specialization constant
#define LL_SPECIALIZATION_BEGIN 4

// default values for the local group size as well as
// indexes for the specialization constants to override them.
layout (
    local_size_x_id = 1, local_size_x = 1,
    local_size_y_id = 2, local_size_y = 1,
    local_size_z_id = 3, local_size_z = 1
) in;

#endif // LLUVIA_CORE_GLSL_
