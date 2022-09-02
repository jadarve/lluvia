#ifndef LLUVIA_CORE_CAMERA_GLSL_
#define LLUVIA_CORE_CAMERA_GLSL_

/**

*/
struct Camera {
    mat3 K;
    mat3 Kinv;
    vec4 radialDistortion;
    vec4 tangentialDistortion;
};


vec2 camera_standardInterpolationCoordinates(const Camera cam, const vec2 pixCoords) {

    const vec3 normalizedCoords = cam.Kinv * vec3(pixCoords.xy, 1);

    ///////////////////////////////////////////////////////
    // Radial distortion
    ///////////////////////////////////////////////////////

    // image plane coordinate radius squared
    const float r2 = dot(normalizedCoords.xy, normalizedCoords.xy);

    // radial distortion coefficient
    const float R = (cam.radialDistortion.x * r2) + 
                    (cam.radialDistortion.y * r2 * r2) + 
                    (cam.radialDistortion.z * r2 * r2 * r2);
    
    // radial distortion coordinates
    const vec2 Pradial = normalizedCoords.xy * (1 + R);

    ///////////////////////////////////////////////////////
    // Tangential distortion
    ///////////////////////////////////////////////////////
    const vec2 Ptangential = vec2(
        (2 * cam.tangentialDistortion.x * normalizedCoords.x * normalizedCoords.y) + cam.tangentialDistortion.y * (R + 2 * normalizedCoords.x * normalizedCoords.x),
        (cam.tangentialDistortion.x * (R + 2 * normalizedCoords.y * normalizedCoords.y) + (2 * cam.tangentialDistortion.y * normalizedCoords.x * normalizedCoords.y))
    );

    // Add both distortions
    const vec3 Pdistorted = vec3(Pradial + Ptangential, 1);

    // Convert back to pixel coordinates
    const vec3 outPixCoords = cam.K * Pdistorted;
    return outPixCoords.xy;
}


vec2 camera_fisheyeInterpolationCoordinates(const Camera cam, const vec2 pixCoords) {

    // TODO
    return pixCoords;
}


#endif // LLUVIA_CORE_CAMERA_GLSL_
