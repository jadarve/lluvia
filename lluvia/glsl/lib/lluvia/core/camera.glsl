#ifndef LLUVIA_CORE_CAMERA_GLSL_
#define LLUVIA_CORE_CAMERA_GLSL_

/**

*/
struct ll_camera {
    mat3 K;
    mat3 Kinv;
    vec4 radialDistortion;
    vec4 tangentialDistortion;
};


vec2 ll_camera_standardInterpolationCoordinates(const ll_camera cam, const vec2 pixCoords) {

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


/**
See See https://docs.opencv.org/3.4/db/d58/group__calib3d__fisheye.html for more details.

FIXME: when the radial coefficients are zero, this model yields pixel coordinates different
to that of the input.
*/
vec2 ll_camera_fisheyeInterpolationCoordinates(const ll_camera cam, const vec2 pixCoords) {

    const vec3 normalizedCoords = cam.Kinv * vec3(pixCoords.xy, 1);

    // unpack the radial distortion coeficients
    const float k1 = cam.radialDistortion.x;
    const float k2 = cam.radialDistortion.y;
    const float k3 = cam.radialDistortion.z;
    const float k4 = cam.radialDistortion.w;

    // image plane coordinate radius squared
    const float r2 = dot(normalizedCoords.xy, normalizedCoords.xy);
    const float r = sqrt(r2);

    // pixel angle
    const float theta = atan(r);

    const float theta2 = theta * theta;
    const float theta4 = theta2 * theta2;
    const float theta6 = theta4 * theta2;
    const float theta8 = theta4 * theta4;

    const float thetaDist = theta * (1 + k1 * theta2 + k2 * theta4 + k3 * theta6 + k4 * theta8);

    // possible division by zero
    const float scale = r > 1e-5? thetaDist / r : 0;

    // distorted coordinates
    const vec3 Pdistorted = vec3(scale * normalizedCoords.xy, 1);

    // Convert back to pixel coordinates
    const vec3 outPixCoords = cam.K * Pdistorted;
    return outPixCoords.xy;
}


#endif // LLUVIA_CORE_CAMERA_GLSL_
