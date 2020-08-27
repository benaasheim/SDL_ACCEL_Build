//
//  Sun.h
//  SDL_ACCEL10
//
//  Created by Ben on 8/13/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Sun_h
#define Sun_h

#include <stdio.h>
#include "Light.h"
#include "Object.h"
#include <simd/simd.h>

typedef struct {
    Light light;
    Object sphere;
} Sun;

Sun newSun(simd_float3 color, simd_float3 position, float intensity, float constant, float linear, float exponent, float radius, int material) {
    Sun sun;
    
    sun.light = newLight(color, position, intensity, constant, linear, exponent);
    sun.sphere = makeSphere(position, radius, material);
    
    return sun;
}

#endif /* Sun_h */

