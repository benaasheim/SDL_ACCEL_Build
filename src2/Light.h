//
//  Light.h
//  SDL_ACCEL9
//
//  Created by Ben on 8/11/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Light_h
#define Light_h

#include <stdio.h>
#include "Vector.h"

//typedef struct {
//    vec position;
//    vec color;
//    float constant;
//    float linear;
//    float exponent;
//} Light;

struct __attribute__ ((__packed__)) Light {
    vec position;
    vec color;
    float constant;
    float linear;
    float exponent;
};

typedef struct Light Light;

Light newLight(vec color, vec position, float intensity, float constant, float linear, float exponent) {
    Light light;
    
    light.position = position;
    mulfv(intensity, &color, &light.color);
    light.constant = constant;
    light.linear = linear;
    light.exponent = exponent;
    
    return light;
}

#endif /* Light_h */

