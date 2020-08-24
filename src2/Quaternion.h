//
//  Quaternion.h
//  SDL_ACCEL9
//
//  Created by Ben on 8/11/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Quaternion_h
#define Quaternion_h

#include "Vector.h"

// Create a new Quaternion with the given values

void QuaternionVT(float * restrict v, float theta, float * restrict r) {
    theta = theta * 3.14 / 360;
    float stheta = sinf(theta);
    mulfv(stheta, v, r);
    r[3] = cosf(theta);
}

void QuaternionVTSelf(float * restrict v, float theta) {
    theta = theta * 3.14 / 360;
    float stheta = sinf(theta);
    mulfvSelf(stheta, v);
    v[3] = cosf(theta);
}

//both quaternions
void mulqq(float * restrict a, float * restrict q, //input vectors
            float * restrict r //result vector
) {
    r[0] = a[3] * q[0] + a[0] * q[3] + a[1] * q[2] - a[2] * q[1];
    r[1] = a[3] * q[1] - a[0] * q[2] + a[1] * q[3] + a[2] * q[0];
    r[2] = a[3] * q[2] + a[0] * q[1] - a[1] * q[0] + a[2] * q[3];
    r[3] = a[3] * q[3] - a[0] * q[0] - a[1] * q[1] - a[2] * q[2];
}

void mulqqSelf(float * restrict a, float * restrict q //input vectors
) {
    q[0] = a[3] * q[0] + a[0] * q[3] + a[1] * q[2] - a[2] * q[1];
    q[1] = a[3] * q[1] - a[0] * q[2] + a[1] * q[3] + a[2] * q[0];
    q[2] = a[3] * q[2] + a[0] * q[1] - a[1] * q[0] + a[2] * q[3];
    q[3] = a[3] * q[3] - a[0] * q[0] - a[1] * q[1] - a[2] * q[2];
}

void mulqv(float * restrict a, float * restrict q, //input vectors
           float * restrict r //result vector
) {
    
    r[3] = -a[0] * q[0] - a[1] * q[1] - a[2] * q[2];
    r[0] = a[3] * q[0] + a[1] * q[2] - a[2] * q[1];
    r[1] = a[3] * q[1] + a[2] * q[0] - a[0] * q[2];
    r[2] = a[3] * q[2] + a[0] * q[1] - a[1] * q[0];
}

void conjugate2(float * restrict a, //input vectors
          float * restrict r //result vector
) {
    mulfv(-1, a, r);
    r[3] = a[3];
}

void conjugate2Self(float * restrict a //input vectors
) {
    
    mulfvSelf(-1, a);
    a[3] *= -1;
}

float qLength(float * restrict a) {
    float sum = 0;
    for (int i = 0; i < SIZE; i++) {
        sum = sum + a[i] * a[i];
    }
    return sqrtf(sum);
}

void normalizeQ(float * restrict q, //input quaternion
          float * restrict r //result quat
) {
    divideFloatSelf(qLength(q), r);
}

void normalizeQSelf(float * restrict q //input quaternion
) {
    

    divideFloatSelf(qLength(q), q);
}

void mulvq(float * restrict q, //input quaternion
           float * restrict v, //input vector
           float * restrict r //result vector
 ) {
     vec q_inv;
     conjugate2(q, &q_inv);
     vec w;
     mulqv(q, v, &w);
     mulqq(&w, &q_inv, r);
 }

void ForwardVector(float * restrict a, //input vectors
                   float * restrict r //result vector)
) {
    r[0] = 2.0f*a[2]*a[0] + 2.0f*a[1]*a[3];
    r[1] = 2.0f*a[2]*a[1] - 2.0f*a[0]*a[3];
    r[2] = a[2]*a[2] - a[1]*a[1] - a[0]*a[0] + a[3]*a[3];
}

void UpVector(float * restrict a, //input vectors
                   float * restrict r //result vector)
) {
    r[0] = 2.0f*a[1]*a[0] - 2.0f*a[2]*a[3];
    r[1] = a[1]*a[1] - a[2]*a[2] + a[3]*a[3] - a[0]*a[0];
    r[2] = 2.0f*a[1]*a[2] + 2.0f*a[0]*a[3];
}

void RightVector(float * restrict a, //input vectors
                   float * restrict r //result vector)
) {
    r[1] = 2.0f*a[1]*a[0] + 2.0f*a[2]*a[3];
    r[2] = 2.0f*a[2]*a[0] - 2.0f*a[1]*a[3];
    r[0] = a[3]*a[3] - a[1]*a[1] - a[2]*a[2] + a[0]*a[0];
}

#endif /* Quaternion_h */

