//
//  Hit.h
//  SDL_ACCEL6
//
//  Created by Ben on 8/6/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Hit_h
#define Hit_h

#include "Vector.h"

typedef struct {
    void* obj;
    vec normal;
    vec position;
    float t;
} Hit;


#endif /* Hit_h */

