//
//  Hit.h
//  SDL_ACCEL10
//
//  Created by Ben on 8/13/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Hit_h
#define Hit_h

#include <simd/simd.h>

struct __attribute__ ((__packed__)) Hit {
    simd_float3 normal;
    simd_float3 position;
};

typedef struct Hit Hit;

#endif /* Hit_h */

