//
//  Light.h
//  SDL_ACCEL13
//
//  Created by Ben on 8/26/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Light_h
#define Light_h

#include <simd/simd.h>

struct __attribute__ ((__packed__)) Light {
    simd_float3 position;
    simd_float3 color;
    float constant;
    float linear;
    float exponent;
};

typedef struct Light Light;

Light newLight(simd_float3 color, simd_float3 position, float intensity, float constant, float linear, float exponent) {
    Light light;
    
    light.position = position;
    light.color = color * intensity;
    light.constant = constant;
    light.linear = linear;
    light.exponent = exponent;
    
    return light;
}

#endif /* Light_h */

