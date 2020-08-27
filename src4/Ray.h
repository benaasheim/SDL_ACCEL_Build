//
//  Ray.h
//  SDL_ACCEL13
//
//  Created by Ben on 8/26/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Ray_h
#define Ray_h

#include <simd/simd.h>

struct __attribute__ ((__packed__)) Ray{
    simd_float3 direction;
    simd_float3 origin;
};

typedef struct Ray Ray;

void printVec(simd_float3 vec) {
    printf("V:{%f %f %f}\n", vec.x, vec.y, vec.z);
}

#endif /* Ray_h */

