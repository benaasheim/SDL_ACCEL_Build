//
//  Hit.h
//  SDL_ACCEL9
//
//  Created by Ben on 8/11/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Hit_h
#define Hit_h

#include "Vector.h"

//typedef struct {
////    void* obj;
//    vec normal;
//    vec position;
//} Hit;

struct __attribute__ ((__packed__)) Hit {
    vec normal;
    vec position;
};

typedef struct Hit Hit;

#endif /* Hit_h */

