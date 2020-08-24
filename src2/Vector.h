//
//  Vector.h
//  SDL_ACCEL9
//
//  Created by Ben on 8/11/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Vector_h
#define Vector_h

#include <stdio.h>
#include <math.h>

#define SIZE (1L << 2)

//typedef struct {
//    float x;
//    float y;
//    float z;
//    float w;
//} vec;

struct __attribute__ ((__packed__)) vec {
    float x;
    float y;
    float z;
    float w;
};

typedef struct vec vec;

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

void zeroV(float * restrict a) {
    for (int i = 0; i < SIZE; i++) {
        a[i] = 0.0f;
    }
}

void negateVec(float * restrict a, float * restrict b) {
    for (int i = 0; i < SIZE; i++) {
        b[i] = -a[i];
    }
}

void negateVecSelf(float * restrict a) {
    for (int i = 0; i < SIZE; i++) {
        a[i] = -a[i];
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

void reflect(float * restrict a, float * restrict b, //inputs
             float * restrict c //result vector
) {
    float f1 = dot(b, a);

    mulfv(f1, b, c);

    mulfvSelf(2.0f, c);
    subVecSelf(a, c);
}

void refract(float * restrict a, float * restrict b, float n, float NdotI, float cos_t, //inputs
             float * restrict c //result vector
) {
    cos_t = sqrtf(1.0f - cos_t);
    
    if (cos_t < 1.0f) {
        vec temp1;
        float tfloat = n*NdotI-cos_t;
        mulfv(tfloat, b, &temp1);
        
        vec temp2;
        mulfv(n, a, c);
        
        addVecSelf(&temp1, c);
    }
    else {
        reflect(a, b, c);
    }
}

void refract2(float * restrict a, float * restrict b, float ior, //inputs
             float * restrict c //result vector
) {
    
    float NdotI, cos_t, tfloat;
    NdotI = dot(a, b);
    
    if (NdotI > 0.0f) {
        negateVecSelf(b);
    }
    else {
        ior = 1/ior;
        NdotI = -NdotI;
    }
    
    cos_t = ior * ior * (1.0f - NdotI * NdotI);
    
    cos_t = sqrtf(1.0f - cos_t);
    
    if (cos_t < 1.0f) {
                
        tfloat = ior*NdotI-cos_t;
        
        mulfvSelf(tfloat, b);
        
        mulfv(ior, a, c);
        
        addVecSelf(b, c);
    }
    else {
        reflect(a, b, c);
    }
}

#endif /* Vector_h */

