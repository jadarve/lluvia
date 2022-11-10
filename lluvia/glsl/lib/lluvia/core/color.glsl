#ifndef LLUVIA_CORE_COLOR_GLSL_
#define LLUVIA_CORE_COLOR_GLSL_

/**
@brief      Converts from RGBA to gray scale

The gray value is computed as:

{code}
uint gray = dot(RGBA, vec4(0.29899999, 0.58700001, 0.114, 0.0))
{code}


@param[in]  RGBA  The rgba value.

@return     The gray value in range [0, 255].
*/
uint color_rgba2gray(const uvec4 RGBA)
{

    return uint(dot(RGBA, vec4(0.29899999, 0.58700001, 0.114, 0.0)));
}

/**
@brief      Converts from RGBA (uint8) to HSVA color space

See https://en.wikipedia.org/wiki/HSL_and_HSV for more details.

@param[in]  RGBA       The rgba color in range [0, 255]
@param[in]  minChroma  The minimum chroma in range [0, 1]

@return     the HSVA value. The values lie within the following ranges.
                * H in [0, 2*pi]
                * S in [0, 1]
                * V in [0, 1]
                * A in [0, 1]
*/
vec4 color_rgba2hsva(const uvec4 RGBA, const float minChroma)
{

    const float r = float(RGBA.r) / 255.0;
    const float g = float(RGBA.g) / 255.0;
    const float b = float(RGBA.b) / 255.0;
    const float a = float(RGBA.a) / 255.0;

    const uint M_ui = max(RGBA.r, max(RGBA.g, RGBA.b));

    const float M = max(r, max(g, b));
    const float m = min(r, min(g, b));
    const float C = M - m;

    // Better compare the integer values RGBA.r, RGBA.g and M_ui than
    // their floating point counter parts. This way, comparison is
    // exact.
    float H = C <= minChroma ? 0 : M_ui == RGBA.r ? mod((g - b) / C, 6)
        : M_ui == RGBA.g                          ? ((b - r) / C) + 2
                                                  : ((r - g) / C) + 4;

    H = radians(60 * H);
    const float V = M;

    const float S = V == 0 ? 0 : C / V;

    return vec4(H, S, V, a);
}

/**
@brief      Converts from HSVA color space to RGBA (uint8)

See https://en.wikipedia.org/wiki/HSL_and_HSV for more details.

@param[in]  HSVA  The hsva color:
                * H in [0, 2*pi]
                * S in [0, 1]
                * V in [0, 1]
                * A in [0, 1]

@return     The RGBA color value. Each component lies in the range [0, 255].
*/
uvec4 color_hsva2rgba(const vec4 HSVA)
{

    const float h = degrees(HSVA.x);
    const float s = HSVA.y;
    const float v = HSVA.z;
    const float a = HSVA.w;

    const float c = v * s;
    const float x = c * (1 - abs(mod(h / 60, 2) - 1));
    const float m = v - c;

    vec3 rgb = 0 <= h && h < 60 ? vec3(c, x, 0) : 60 <= h && h < 120 ? vec3(x, c, 0)
        : 120 <= h && h < 180                                        ? vec3(0, c, x)
        : 180 <= h && h < 240                                        ? vec3(0, x, c)
        : 240 <= h && h < 300                                        ? vec3(x, 0, c)
                                                                     : vec3(c, 0, x);

    // convert rgb range from [0, 1] to [0, 255]
    rgb = clamp(255 * (rgb + m), 0, 255);
    uint alpha = uint(clamp(255 * a, 0, 255));

    return uvec4(rgb.r, rgb.g, rgb.b, alpha);
}

#endif // LLUVIA_CORE_COLOR_GLSL_
