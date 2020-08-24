//
//  Ray.h
//  SDL_ACCEL9
//
//  Created by Ben on 8/11/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Ray_h
#define Ray_h

#include "Vector.h"

//typedef struct {
//    vec direction;
//    vec origin;
//} Ray;

struct __attribute__ ((__packed__)) Ray{
    vec direction;
    vec origin;
};

typedef struct Ray Ray;

#endif /* Ray_h */

