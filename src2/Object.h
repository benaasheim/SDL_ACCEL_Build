//
//  Object.h
//  SDL_ACCEL9
//
//  Created by Ben on 8/11/20.
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

//typedef struct Object {
//    vec c; //center of Circle, center/position of Plane
//    vec n; //xCoord is radius of Circle, normal of Plane
//    vec z; //third vertex of triangle
//    void (*interesect)(Ray* ray, struct Object* obj, float t, Hit* hit); //intersection method
//    float (*gett)(Ray* ray, struct Object* obj); //intersection method
//    int mat; //photo-material reference in scene materials-list
//} Object;

struct __attribute__ ((__packed__)) Object {
    vec c; //center of Circle, center/position of Plane
    vec n; //xCoord is radius of Circle, normal of Plane
    vec z; //third vertex of triangle
    void (*interesect)(Ray* ray, struct Object* obj, float t, Hit* hit); //intersection method
    float (*gett)(Ray* ray, struct Object* obj); //intersection method
    int mat; //photo-material reference in scene materials-list
};

typedef struct Object Object;

float calcPlaneT(Ray* ray, struct Object* plane) {
    
    float d = dot(&plane->n, &ray->direction);
    
    if (d > 0.0f) {
        return -1.0f;
    }
        
    vec P;
    subVec(&plane->c, &ray->origin, &P);
    
    float t = dot(&P, &plane->n)/d;
    
    return t;
}

void calcIntersectionPlane2(Ray* ray, struct Object* plane, float t, Hit* hit) {
    mulfv(t, &ray->direction, &hit->position);
    addVec(&ray->origin, &hit->position, &hit->position);
    normalize(&plane->n, &hit->normal);
//    hit->obj = plane;
}

float calcSphereT(Ray* ray, struct Object* sphere) {
    vec SP;
    float t, b, d;
    
    subVec(&sphere->c, &ray->origin, &SP);
    
    b = dot(&SP, &ray->direction);
    
    d = b * b - dotSelf(&SP) + squareff(sphere->n.x);
    
    if (d < 0.0f) {
        return -1.0f;
    }
    
    d = sqrtf(d);
    
    t = (t = b - d) > epsilon ? t : ((t = b + d) > epsilon ? t : -1.0f);
    
    return t;
}

void calcIntersectionSphere2(Ray* ray, struct Object* sphere, float t, Hit* hit) {
    mulfv(t, &ray->direction, &hit->position);
    addVec(&ray->origin, &hit->position, &hit->position);
    subVec(&hit->position, &sphere->c, &hit->normal);
    divideFloat(&hit->normal, sphere->n.x, &hit->normal);
    
//    hit->obj = sphere;
}

Object makeObject(vec center, vec p2, vec p3, Hit* (*inter)(Ray* ray, struct Object* obj, float t), float (*gt)(Ray* ray, struct Object* obj), int material) {
    Object object;
    object.c = center;
    object.n = p2;
    object.z = p3;
    object.interesect = inter;
    object.gett = gt;
    object.mat = material;
    return object;
}

Object makeSphere(vec center, float radius, int material) {
    Object object;
    vec oth = {radius, 0, 0};
    Hit* (*inter)(Ray* ray, struct Object* obj, float t) = calcIntersectionSphere2;
    float (*gt)(Ray* ray, struct Object* obj) = calcSphereT;
    object = makeObject(center, oth, oth, inter, gt, material);
    return object;
}

Object makePlane(vec center, vec radius, int material) {
    Object object;
    Hit* (*inter)(Ray* ray, struct Object* obj, float t) = calcIntersectionPlane2;
    float (*gt)(Ray* ray, struct Object* obj) = calcPlaneT;
    vec oth = {0, 0, 0};
    object = makeObject(center, radius, oth, inter, gt, material);
    return object;
}

#endif /* Object_h */

