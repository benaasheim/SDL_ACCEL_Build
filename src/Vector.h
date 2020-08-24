//
//  Vector.h
//  SDL_ACCEL6
//
//  Created by Ben on 8/6/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Vector_h
#define Vector_h

#include <stdio.h>
#include <math.h>

#define SIZE (1L << 2)

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec;

void printVec(vec v) {
    printf("V:{%f,%f,%f}\n", v.x, v.y, v.z);
}

void addVec(float * restrict a, float * restrict b, //inputs
            float * restrict c //result vector
) {
    for (int i = 0; i < SIZE; i++) {
        c[i] = b[i] + a[i];
    }
}

void addVecSelf(float * restrict a, //inputs
                float * restrict b //result vector
) {
    for (int i = 0; i < SIZE; i++) {
        b[i] += a[i];
    }
}

vec addV(float * restrict a, float * restrict b) {
    vec cc;
    float * restrict c = &cc;
    for (int i = 0; i < SIZE; i++) {
        c[i] = b[i] + a[i];
    }
    return cc;
}

void subVec(float * restrict a, float * restrict b, //inputs
            float * restrict c //result vector
) {
    for (int i = 0; i < SIZE; i++) {
        c[i] = a[i] - b[i];
    }
}

void subVecSelf(float * restrict a, //inputs
                float * restrict b //result vector
) {
    for (int i = 0; i < SIZE; i++) {
        b[i] = a[i] - b[i];
    }
}

float dot(float * restrict a, float * restrict b
          ) {
    a[3] = 0;
    b[3] = 0;
    float sum = 0;
    for (int i = 0; i < SIZE; i++) {
        sum = sum + b[i] * a[i];
    }
    return sum;
}

float dotSelf(float * restrict a //inputs
) {
    a[3] = 0;
    float sum = 0;
    for (int i = 0; i < SIZE; i++) {
        sum = sum + a[i] * a[i];
    }
    return sum;
}

float mag(float * restrict a) {
    return sqrt(dotSelf(a));
}

void mulfv(float value, float * restrict a, //inputs
           float * restrict b //result vector
) {
    for (int i = 0; i < SIZE; i++) {
        b[i] = value * a[i];
    }
}

void mulfvSelf(float value, float * restrict a
) {
    for (int i = 0; i < SIZE; i++) {
        a[i] *= value;
    }
}

void mulvv(float * restrict a, float * restrict b, //input vectors
           float * restrict c //result vector
) {
    for (int i = 0; i < SIZE; i++) {
        c[i] = b[i] * a[i];
    }
}

void mulvvSelf(float * restrict a, float * restrict b
) {
    for (int i = 0; i < SIZE; i++) {
        b[i] *= a[i];
    }
}

void divideFloat(float * restrict a, float f, //input vectors
                 float * restrict b //result vector
                 ) {
    for (int i = 0; i < SIZE; i++) {
        b[i] = a[i] / f;
    }
}

void divideFloatSelf(float f, float * restrict b
) {
    for (int i = 0; i < SIZE; i++) {
        b[i] /= f;
    }
}

void normalize(float * restrict a, //input vector
               float * restrict b //result vector
) {
    divideFloat(a, mag(a), b);
}

void normalizeSelf(float * restrict a //input vector
) {
    divideFloatSelf(mag(a), a);
}

void reflect(float * restrict a, float * restrict b, //inputs
             float * restrict c //result vector
) {
    mulfv(2 * dot(b, a), b, c);
    subVecSelf(a, c);
}

void reflect2(float * restrict a, float * restrict b, //inputs
             float * restrict c //result vector
) {
//    mulfv(2 * dot(b, a), b, c);
//    subVecSelf(a, c);
    float f1 = dot(b, a);
    vec temp;
    mulfv(f1, b, &temp);
    
    vec temp1;
    mulfv(2.0f, &temp, &temp1);
    
    subVec(a, &temp1, c);
}


void refract(float * restrict a, float * restrict b, float ior, float NdotI, //inputs
             float * restrict c //result vector
) {
    
    if (NdotI > 0.0f) {
        mulfvSelf(-1, b);
    }
    else {
        ior = 1.0f / ior;
        NdotI = -NdotI;
    }
    float cos_t = sqrtf(1.0f - ior * ior * (1.0f - NdotI * NdotI));
    if (cos_t < 1.0f) {
        float cc = ior * NdotI - cos_t;
        vec v;
        mulfv(ior, a, &v);
        mulfv(cc, b, c);
        addVecSelf(&v, c);
    }
    else {
        return reflect(a, b, c);
    }
}

void refract2(float * restrict a, float * restrict b, float n, float NdotI, float cos_t, //inputs
             float * restrict c //result vector
) {
    cos_t = sqrtf(1.0f - cos_t);
    
    if (cos_t < 1.0f) {
        vec temp1;
        float tfloat = n*NdotI-cos_t;
        mulfv(tfloat, b, &temp1);
        
        vec temp2;
        mulfv(n, a, &temp2);
        
        addVec(&temp1, &temp2, c);
    }
    else {
        reflect2(a, b, c);
    }
}

void zeroV(float * restrict a) {
    for (int i = 0; i < SIZE; i++) {
        a[i] = 0.0f;
    }
}

void quickclampVector(float * restrict a,
                      float * restrict b
) {
    for (int i = 0; i < SIZE; i++) {
        b[i] = fmaxf(0, fminf(a[i], 1));
    }
}

void quickclampVectorSelf(float * restrict a
) {
    for (int i = 0; i < SIZE; i++) {
        a[i] = fmaxf(0, fminf(a[i], 1));
    }
}

float averageVector(float * restrict a
                    ) {
    float sum = 0;
    a[3] = 0;
    for (int i = 0; i < SIZE; i++) {
        sum += a[i];
    }
    return sum/3;
}

void setA2B(float * restrict a, //inputs
         float * restrict b //result vector
) {
    for (int i = 0; i < SIZE; i++) {
        b[i] = a[i];
    }
}

#endif /* Vector_h */

