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
uint rgba2gray(const uvec4 RGBA) {

    return uint(dot(RGBA, vec4(0.29899999, 0.58700001, 0.114, 0.0)));
}


/**
@brief      Converts from RGBA to HSVA color space.


The HSV values are computed as:

{code}
V = max(R, G, B);
S = V == 0.0? 0 : (V - min(R, G, B)) / V;


H = V == R ?       60*(G - B) / (V - min(R, G, B)) :
    V == G ? 120 + 60*(B - R) / (V - min(R, G, B)) :
             240 + 60*(R - G) / (V - min(R, G, B));
{code}

The alpha channel is passed through to the output without
modifiations.


@param[in]  RGBA  The rgba value.

@return     The HSVA value.
*/
uvec4 rgba2hsva(const uvec4 RGBA) {

    // // RGBA values as normalized floating point
    // const float R = RGBA.r / 255.0;
    // const float G = RGBA.g / 255.0;
    // const float B = RGBA.b / 255.0;
    // const float A = RGBA.a;
    
    // const float minRGB = min(R, min(G, B));
    
    // const float V = max(R, max(G, B));
    // const float S = V == 0.0? 0 : (V - minRGB) / V;
    
    // const float denominator = V - minRGB;
    
    // const float H = V == R ?       60*(G - B) / denominator :
    //                 V == G ? 120 + 60*(B - R) / denominator :
    //                          240 + 60*(R - G) / denominator;
    
    // return uvec4(0.5*H, 255.0*S, 255.0*V, A);

    // "optimized" version
    const uint V = max(RGBA.r, max(RGBA.g, RGBA.b));
    
    const uint  minRGB       = min(RGBA.r, min(RGBA.g, RGBA.b));
    const float VminusMinRGB = V - minRGB;

    const uint S = V == 0? 0 : uint(255*(VminusMinRGB / V));
    
    const float normFactor = 30 / VminusMinRGB;
    
    const int Ri = int(RGBA.r);
    const int Gi = int(RGBA.g);
    const int Bi = int(RGBA.b);

    const uint H =  V == RGBA.r ?       uint(normFactor*(Gi - Bi)) :
                    V == RGBA.g ? 60  + uint(normFactor*(Bi - Ri)) :
                                  120 + uint(normFactor*(Ri - Gi));
    
    return uvec4(H, S, V, RGBA.a);
}

#endif // LLUVIA_CORE_COLOR_GLSL_
