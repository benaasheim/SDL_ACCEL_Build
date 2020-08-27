//
//  Object.h
//  SDL_ACCEL13
//
//  Created by Ben on 8/26/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Object_h
#define Object_h

#include <stdio.h>
#include <math.h>
#include "Hit.h"
#include "Settings.h"
#include "Materials.h"
#include "Ray.h"

#define squareff(f) (f * f)

struct __attribute__((__packed__)) Object {
    simd_float3 c; //center of Circle, center/position of Plane
    simd_float3 n; //xCoord is radius of Circle, normal of Plane
    simd_float3 z; //third vertex of triangle
//    Hit
//    simd_float3
// (*interesect)(const simd_float3 A, const simd_float3 B, const struct Object* obj, const float t); //intersection method
    char type;
    float (*gett)(const Ray ray, const struct Object* obj); //intersection method
    int mat; //photo-material reference in scene materials-list
};

typedef struct Object Object;

float calcPlaneT(const Ray ray, const struct Object* plane) {
    
    float d = simd_dot(plane->n, ray.direction);
    
    if (d > 0.0f) {
        return -1.0f;
    }
        
    simd_float3 P = plane->c - ray.origin;
    
    float t = simd_dot(P, plane->n)/d;
    
    return t;
}

Hit calcIntersectionPlane(const Ray ray, const struct Object* plane, const float t) {
    Hit hit;
    hit.position = ray.origin + (ray.direction * t);
    hit.normal = plane->n;
    return hit;
}

simd_float3 calcIntersectionPlane2(const simd_float3 position, const simd_float3 p1,  const struct Object* plane, const float t) {
    return plane->n;
}

#define plane__normal \
else if (objf->type == 'p') { \
    xFinal.normal = objf->n; \
} \

float calcSphereT(const Ray ray, const struct Object* sphere) {
    simd_float3 SP;
    float t, b, d;
    
    SP = sphere->c - ray.origin;
    
    b = simd_dot(SP, ray.direction);
    
    d = b * b - simd_dot(SP, SP) + squareff(sphere->n.x);
    
    if (d < 0.0f) {
        return -1.0f;
    }
    
    d = sqrtf(d);
    
    t = (t = b - d) > epsilon ? t : ((t = b + d) > epsilon ? t : -1.0f);
    return t;
}

Hit calcIntersectionSphere(const Ray ray, const struct Object* sphere, const float t) {
    Hit hit;
    hit.position = ray.origin + (ray.direction * t);
    hit.normal = (hit.position - sphere->c)/sphere->n.x;
    return hit;
}

simd_float3 calcIntersectionSphere2(const simd_float3 position, const simd_float3 p1, const struct Object* sphere, const float t) {
    return (position - sphere->c)/sphere->n.x;
}

#define sphere__normal \
else if (objf->type == 's') { \
    xFinal.normal = (xFinal.position - objf->c)/objf->n.x; \
} \

Object makeObject(simd_float3 center, simd_float3 p2, simd_float3 p3, simd_float3 (*inter)(const simd_float3 position, const simd_float3 raydirection, const struct Object* triangle, const float t), float (*gt)(const Ray ray, const struct Object* obj), int material, char type) {
    Object object;
    object.c = center;
    object.n = p2;
    object.z = p3;
//    object.interesect = inter;
    object.gett = gt;
    object.mat = material;
    object.type = type;
    return object;
}

Object makeSphere(simd_float3 center, float radius, int material) {
    Object object;
    simd_float3 oth = {radius, 0, 0};
    simd_float3 (*inter)(const simd_float3 position, const simd_float3 raydirection,  const struct Object* triangle, const float t) = calcIntersectionSphere2;
    float (*gt)(const Ray ray, const struct Object* obj) = calcSphereT;
    object = makeObject(center, oth, oth, inter, gt, material, 's');
    return object;
}

Object makePlane(simd_float3 center, simd_float3 radius, int material) {
    Object object;
    simd_float3 (*inter)(const simd_float3 position, const simd_float3 raydirection,  const struct Object* triangle, const float t) = calcIntersectionPlane2;
    float (*gt)(const Ray ray, const struct Object* obj) = calcPlaneT;
    simd_float3 oth = {0, 0, 0};
    object = makeObject(center, radius, oth, inter, gt, material, 'p');
    return object;
}

float calcTriangleT(const Ray ray, const struct Object* triangle) {
    
    simd_float3 u = triangle->n - triangle->c;
    simd_float3 v = triangle->z - triangle->c;
    simd_float3 w = triangle->c - ray.origin;
    simd_float3 plane1n = simd_normalize(simd_cross(u, v));
    
    float d = simd_dot(plane1n, ray.direction);
    
    if (d > 0.0f) {
        plane1n *= -1;
        d *= -1;
    }
    
    float t = simd_dot(w, plane1n)/d;
    
    w = ray.direction * t - w;
    float uDv = simd_dot(u, v);
    float wDv = simd_dot(w, v);
    float vDv = simd_dot(v, v);
    float wDu = simd_dot(w, u);
    float uDu = simd_dot(u, u);
    
    float s1 = (uDv*wDv - vDv*wDu)/(uDv*uDv - uDu*vDv);
    float t1 = (uDv*wDu - uDu*wDv)/(uDv*uDv - uDu*vDv);
    
    if (s1 >= 0.0f && t1 >= 0.0f && s1 + t1 < 1.0f) {
        return t;
    }
    
    return -1.0;
}

Hit calcIntersectionTriangle(const Ray ray, const struct Object* triangle, const float t) {
    Hit hit;
    hit.normal = simd_normalize(simd_cross((triangle->n - triangle->c), (triangle->z - triangle->c)));
    float d = simd_dot(hit.normal, ray.direction);
    if (d > 0.0f) {
        hit.normal *= -1;
    }
    hit.position = ray.origin + (ray.direction * t);
    return hit;
}

simd_float3 calcIntersectionTriangle2(const simd_float3 raydirection,  const struct Object* triangle, const float t) {
    simd_float3 hitnormal;
    hitnormal = simd_normalize(simd_cross((triangle->n - triangle->c), (triangle->z - triangle->c)));
    float d = simd_dot(hitnormal, raydirection);
    if (d > 0.0f) {
        hitnormal *= -1;
    }
    return hitnormal;
}

#define triangle__normal \
if (objf->type == 't') { \
    xFinal.normal = simd_normalize(simd_cross((objf->n - objf->c), (objf->z - objf->c))); \
float d = simd_dot(xFinal.normal, ray.direction); \
    if (d > 0.0f) { \
        xFinal.normal *= -1; \
    } \
} \

Object makeTriangle(simd_float3 a, simd_float3 b, simd_float3 c, int material) {
    Object object;
    simd_float3 (*inter)(const simd_float3 raydirection,  const struct Object* triangle, const float t) = calcIntersectionTriangle2;
    float (*gt)(const Ray ray, const struct Object* obj) = calcTriangleT;
    object = makeObject(a, b, c, inter, gt, material, 't');
    return object;
}


#define hit_normal \
triangle__normal \
sphere__normal \
plane__normal \

#endif /* Object_h */

