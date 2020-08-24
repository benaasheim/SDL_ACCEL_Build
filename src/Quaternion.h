//
//  Quaternion.h
//  SDL_ACCEL6
//
//  Created by Ben on 8/6/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Quaternion_h
#define Quaternion_h

#include "Vector.h"

// Create a new Quaternion with the given values
vec newQuaternionVT(vec v, float theta) {
    theta = theta * 3.14 / 360;
    float stheta = sinf(theta);
    
    vec result;
    mulfv(stheta, &v, &result);
    result.w = cosf(theta);
    return result;
}

void QuaternionVT(float * restrict v, float theta, float * restrict r) {
    theta = theta * 3.14 / 360;
    float stheta = sinf(theta);
    mulfv(stheta, v, r);
    r[3] = cosf(theta);
}

//both quaternions
void mulqq(float * restrict a, float * restrict q, //input vectors
            float * restrict r //result vector
            ) {
                r[0] = a[3] * q[0] + a[0] * q[3] + a[1] * q[2] - a[2] * q[1];
                r[1] = a[3] * q[1] - a[0] * q[2] + a[1] * q[3] + a[2] * q[0];
                r[2] = a[3] * q[2] + a[0] * q[1] - a[1] * q[0] + a[2] * q[3];
                r[3] = a[3] * q[3] - a[0] * q[0] - a[1] * q[1] - a[2] * q[2];
                
                
//                for (int i = 0; i < SIZE; i++) {
//                    for (int j = 0; j < SIZE; j++) {
//
//                        for (int k = 0; k < SIZE; k++) {
//                            r[i] += a[j] * q[j] * negs[i][j];
//                        }
//                    }
//                }
}

//vec multVec(vec a, //the quaternion
//            vec v  //the proper vector
//            ) {
//    vec r;
//
//    r.w = -a.x * v.x - a.y * v.y - a.z * v.z;
//    r.x = a.w * v.x + a.y * v.z - a.z * v.y;
//    r.y = a.w * v.y + a.z * v.x - a.x * v.z;
//    r.z = a.w * v.z + a.x * v.y - a.y * v.x;
//
//    return r;
//}

void mulqv(float * restrict a, float * restrict q, //input vectors
           float * restrict r //result vector
            ) {
                r[3] = -a[0] * q[0] - a[1] * q[1] - a[2] * q[2];
                r[0] = a[3] * q[0] + a[1] * q[2] - a[2] * q[1];
                r[2] = a[3] * q[1] + a[2] * q[0] - a[0] * q[2];
                r[3] = a[3] * q[2] + a[0] * q[1] - a[1] * q[0];
}

//vec conjugate(vec a) {
////    return newQuaternion(a.w, -a.x, -a.y, -a.z);
//    mulfv(-1, &a, &a);
//    a.w = -a.w;
//    return a;
//}

void conjugate2(float * restrict a, //input vectors
          float * restrict r //result vector
) {
    mulfv(-1, &a, &r);
    r[3] = a[3];
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

//vec mulByVec(vec a, vec v) {
////    Quaternion q_inv = conjugate(a);
////    Quaternion w = multQuat(multVec(a, v), q_inv);
//    vec q_inv = conjugate(a);
//    vec w = multVec(a, v);
//    mulqq(&w, &q_inv, &w);
//    return w;
//}

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
     

//vec getForwardVector(vec a) {
////    return mulByVec(a, newVector(0, 0, 1));
//    vec rr;
//
//    rr.x = 2.0f*a.z*a.x + 2.0f*a.y*a.w;
//    rr.y = 2.0f*a.z*a.y - 2.0f*a.x*a.w;
//    rr.z = a.z*a.z - a.y*a.y - a.x*a.x + a.w*a.w;
//
//    return rr;
//}

void ForwardVector(float * restrict a, //input vectors
                   float * restrict r //result vector)
) {
    r[0] = 2.0f*a[2]*a[0] + 2.0f*a[1]*a[3];
    r[1] = 2.0f*a[2]*a[1] - 2.0f*a[0]*a[3];
    r[2] = a[2]*a[2] - a[1]*a[1] - a[0]*a[0] + a[3]*a[3];
    r[3] = 0;
}

//vec getUpVector(vec a) {
////    return mulByVec(a, newVector(0, 1, 0));
//    vec rr;
//
//    rr.x = 2.0f*a.y*a.x - 2.0f*a.z*a.w;
//    rr.y = a.y*a.y - a.z*a.z + a.w*a.w - a.x*a.x;
//    rr.z = 2.0f*a.y*a.z + 2.0f*a.x*a.w;
//
//    return rr;
//}

void UpVector(float * restrict a, //input vectors
                   float * restrict r //result vector)
) {
    r[0] = 2.0f*a[1]*a[0] - 2.0f*a[2]*a[3];
    r[1] = a[1]*a[1] - a[2]*a[2] + a[3]*a[3] - a[0]*a[0];
    r[2] = 2.0f*a[1]*a[2] + 2.0f*a[0]*a[3];
}

//vec getRightVector(vec a) {
////    return mulByVec(a, newVector(1, 0, 0));
//    vec rr;
//
//    rr.x = a.x*a.x + a.w*a.w - a.z*a.z - a.y*a.y;
//    rr.y = 2.0f*a.x*a.y + 2.0f*a.w*a.z;
//    rr.z = 2.0f*a.x*a.z - 2.0f*a.w*a.y;
//
//    return rr;
//}

void RightVector(float * restrict a, //input vectors
                   float * restrict r //result vector)
) {
    r[1] = 2.0f*a[1]*a[0] + 2.0f*a[2]*a[3];
    r[2] = 2.0f*a[2]*a[0] - 2.0f*a[1]*a[3];
    r[0] = a[3]*a[3] - a[1]*a[1] - a[2]*a[2] + a[0]*a[0];
}

#endif /* Quaternion_h */

