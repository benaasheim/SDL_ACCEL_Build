//
//  Object.h
//  SDL_ACCEL10
//
//  Created by Ben on 8/13/20.
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

struct __attribute__ ((__packed__)) Object {
    simd_float3 c; //center of Circle, center/position of Plane
    simd_float3 n; //xCoord is radius of Circle, normal of Plane
    simd_float3 z; //third vertex of triangle
    Hit (*interesect)(Ray ray, struct Object* obj, float t); //intersection method
    float (*gett)(Ray ray, struct Object* obj); //intersection method
    int mat; //photo-material reference in scene materials-list
};

typedef struct Object Object;

float calcPlaneT(Ray ray, struct Object* plane) {
    
    float d = simd_dot(plane->n, ray.direction);
    
    if (d > 0.0f) {
        return -1.0f;
    }
        
    simd_float3 P = plane->c - ray.origin;
    
    float t = simd_dot(P, plane->n)/d;
    
    return t;
}

Hit calcIntersectionPlane2(Ray ray, struct Object* plane, float t) {
    Hit hit;
    hit.position = ray.origin + (ray.direction * t);
    hit.normal = plane->n;
    return hit;
}

float calcSphereT(Ray ray, struct Object* sphere) {
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

Hit calcIntersectionSphere2(Ray ray, struct Object* sphere, float t) {
    Hit hit;
    hit.position = ray.origin + (ray.direction * t);
    hit.normal = (hit.position - sphere->c)/sphere->n.x;
    return hit;
}



Object makeObject(simd_float3 center, simd_float3 p2, simd_float3 p3, Hit (*inter)(Ray ray, struct Object* obj, float t), float (*gt)(Ray ray, struct Object* obj), int material) {
    Object object;
    object.c = center;
    object.n = p2;
    object.z = p3;
    object.interesect = inter;
    object.gett = gt;
    object.mat = material;
    return object;
}

Object makeSphere(simd_float3 center, float radius, int material) {
    Object object;
    simd_float3 oth = {radius, 0, 0};
    Hit (*inter)(Ray ray, struct Object* obj, float t) = calcIntersectionSphere2;
    float (*gt)(Ray ray, struct Object* obj) = calcSphereT;
    object = makeObject(center, oth, oth, inter, gt, material);
    return object;
}

Object makePlane(simd_float3 center, simd_float3 radius, int material) {
    Object object;
    Hit (*inter)(Ray ray, struct Object* obj, float t) = calcIntersectionPlane2;
    float (*gt)(Ray ray, struct Object* obj) = calcPlaneT;
    simd_float3 oth = {0, 0, 0};
    object = makeObject(center, radius, oth, inter, gt, material);
    return object;
}

float calcTriangleT2(Ray ray, struct Object* triangle) {
    
    simd_float3 plane1n = simd_normalize(simd_cross((triangle->n - triangle->c), (triangle->z - triangle->c)));
    
    
    float d = simd_dot(plane1n, ray.direction);
    if (d > 0.0f) {
        plane1n *= -1;
        d *= -1;
    }
    float t = simd_dot(triangle->c - ray.origin, plane1n)/d;
    
    simd_float3 u = triangle->n - triangle->c;
    simd_float3 v = triangle->z - triangle->c;
    simd_float3 w = (ray.origin + (ray.direction * t)) - triangle->c;
    
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

Hit calcIntersectionTriangle2(Ray ray, struct Object* triangle, float t) {
    simd_float3 plane1n = simd_normalize(simd_cross((triangle->n - triangle->c), (triangle->z - triangle->c)));
    
    Hit hit;
    
    float d = simd_dot(plane1n, ray.direction);
    if (d > 0.0f) {
        plane1n *= -1;
    }
    hit.position = ray.origin + (ray.direction * t);
    hit.normal = plane1n;
    return hit;
}

float calcTriangleT(Ray ray, struct Object* triangle) {
    
    Object plane1 = makePlane(triangle->c,  simd_normalize(simd_cross((triangle->n - triangle->c), (triangle->z - triangle->c))), triangle->mat);
    Object plane2 = plane1;
    plane2.n *= -1;
    Hit hit;
    
    float t = calcPlaneT(ray, &plane1);
    
    
    
    
    if (t < 0) {
        
        
        
        
        t = calcPlaneT(ray, &plane2);
        if (t < 0) {
            return -1;
        }
        else {
            hit = calcIntersectionPlane2(ray, &plane2, t);
        }
    }
    else {
        hit = calcIntersectionPlane2(ray, &plane1, t);
    }
    
    
    simd_float3 P1 = hit.position;
    simd_float3 V0 = triangle->c;
    simd_float3 V1 = triangle->n;
    simd_float3 V2 = triangle->z;
    
    simd_float3 u = V1 - V0;
    simd_float3 v = V2 - V0;
    simd_float3 w = P1 - V0;
    
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
    else return -1.0;
}

Hit calcIntersectionTriangle(Ray ray, struct Object* triangle, float t) {
    
    Object plane1 = makePlane(triangle->c,  simd_normalize(simd_cross((triangle->n - triangle->c), (triangle->z - triangle->c))), triangle->mat);
    Object plane2 = plane1;
    plane2.n *= -1;
    Hit hit;
    
    float t1 = calcPlaneT(ray, &plane1);
    float t2 = calcPlaneT(ray, &plane2);
    
    if (t1 != -1) {
        return calcIntersectionPlane2(ray, &plane1, t1);
    }
    else {
        return calcIntersectionPlane2(ray, &plane2, t2);
    }
}

Object makeTriangle(simd_float3 a, simd_float3 b, simd_float3 c, int material) {
    Object object;
    Hit (*inter)(Ray ray, struct Object* obj, float t) = calcIntersectionTriangle2;
    float (*gt)(Ray ray, struct Object* obj) = calcTriangleT2;
    object = makeObject(a, b, c, inter, gt, material);
    return object;
}

#endif /* Object_h */

