//
//  Tracer.h
//  SDL_ACCEL6
//
//  Created by Ben on 8/6/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Tracer_h
#define Tracer_h

#include <stdio.h>
#include "Camera.h"
#include "Scene.h"
#include "Settings.h"

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2/SDL_timer.h>

#define float_max 99999999

typedef struct {
    Camera camera;
    Scene scene;
    int thread_amount;
    SDL_bool keyboard_keys[255];
} Tracer;

Tracer newTracer(void (*initmode)(Scene* scene)) {
    Tracer tracer;
    tracer.thread_amount = threads;
    vec cvec = {0, 1, 0};
    tracer.camera = newCamera(cvec, camera_speed, camera_sensitivity);
    tracer.scene = newScene(initmode);
    for (int i = 0; i < 255; i++) {
        tracer.keyboard_keys[i] = SDL_FALSE;
    }
    return tracer;
}

void update_Tracer(Tracer* tracer, double dt) {
//    printf("DT: %f\n", dt);
    update_camera(&tracer->camera, dt, tracer->keyboard_keys);
    update_Scene(&tracer->scene, dt);
}

void handle_key_pressed(SDL_Event event, SDL_bool val, Tracer* tracer) {
    // keyboard API for key pressed
    tracer->keyboard_keys[event.key.keysym.scancode] = val;
}

Hit* basicLoop1(Ray ray, float L_length, Scene* scene) {
    //find nearest object intersected by ray
    Object* obj = NULL;
    Object* objf = NULL;
    float t;
    
    for (int i = 0; i < scene->numObjects; i++) {
        obj = &scene->objects[i];

        t = obj->gett(ray, obj);

        if (t > 0.0f && t < L_length) {
            L_length = t;
            objf = obj;
        }
    }
    if (objf != NULL) {
        return objf->interesect(ray, objf, L_length);
    } else {
        return NULL;
    }
}

Hit* basicLoop12(Ray ray, Scene* scene, Object* thisobj, float L_length) {
    //find nearest object intersected by ray
    Object* obj;
    Object* objf = NULL;
    float t;
    
//     = mag(&ray.direction);
    for (int i = 0; i < scene->numObjects; i++) {
        obj = &scene->objects[i];
    //        Hit* hit = obj->interesect(ray, obj);
        if (thisobj != obj) {
            t = obj->gett(ray, obj);

            if (t > 0.0f && t < L_length) {
                L_length = t;
                objf = obj;
            }
        }
    }
    if (objf != NULL) {
        return objf->interesect(ray, objf, L_length);
    } else {
        return NULL;
    }
}


//float traceShadow(Ray ray, Object* thisobj, Scene* scene, Light light) {
//    Hit* xFinal = NULL;
//    Material* m;
//
////    xFinal = basicLoop12(ray, scene, thisobj);
//
//    if (xFinal == NULL) {
//        return 1.0f;
//    }
//
//    m = &scene->materials[((Object*)(xFinal->obj))->mat];
//    vec tempcol
////    ;
////    vec xn
//    = xFinal->normal;
//    vec xp = xFinal->position;
//    m->shading_func(&ray.direction, &tempcol, &xp, light, &tempcol);
//    float weight = averageVector(&tempcol);
//
//    if (m->reflectivity > 0.0f) {
//        weight -= m->reflectivity;
//    }
//
//    if (m->refractivity > 0.0f) {
//        weight *= m->refractivity;
//    }
//
//    return weight;
//}

float traceShadow2(Ray ray, Object* thisobj, Scene* scene, Light light, float L_length) {
    Hit* xFinal = NULL;
    Material* m;
    
    xFinal = basicLoop12(ray, scene, thisobj, L_length);
    
    if (xFinal == NULL) {
        return 1.0f;
    }

    m = &scene->materials[((Object*)(xFinal->obj))->mat];
//    vec tempcol
////    ;
////    vec xn
//    = xFinal->normal;
//    vec xp = xFinal->position;
//    m->shading_func(&ray.direction, &tempcol, &xp, light, &tempcol);
    vec tempcol = {0, 0, 0};
    Hit XF = *xFinal;
//    tempcol = m->shading_func(ray, XF, light, *m, NULL);
    
    m->shading_func(ray, XF, light, &tempcol);
    float weight = averageVector(&tempcol);
    
    if (m->reflectivity > 0.0f) {
        weight -= m->reflectivity;
    }
    
    if (m->refractivity > 0.0f) {
        weight *= m->refractivity;
    }

    return weight;
}

vec traceColor(Ray r, Scene* scene, int n) {
    vec cFinal = {0.0f, 0.0f, 0.0f};
    Hit* xFinal = NULL;

    
    if (n > max_recursion) {
        return cFinal;
    }

    //find nearest object intersected by ray
    xFinal = basicLoop1(r, float_max, scene);

//    return black if no objects intersected
    if (xFinal == NULL) {
        return cFinal;
    }

    Ray next_ray;
    Hit XF;
    XF.obj = xFinal->obj;
    XF.normal = xFinal->normal;
    XF.position = xFinal->position;
//    XF.t = xFinal->t;

    Light l;

    next_ray.origin = XF.position;
    Material m = scene->materials[((Object*)(XF.obj))->mat];

    for (int i = 0; i < scene->numLights; i++) {
        l = scene->lights[i];
        
        vec temp1 = {0, 0, 0};
        
//        m.shading_func(&r.direction, &XF.normal, &XF.position, l, &temp1);

//        temp1 = m.shading_func(r, XF, l, m, NULL);
        m.shading_func(r, XF, l, &temp1);
        addVecSelf(&temp1, &cFinal);
        
        Ray ray_shadow;
        
//        printVec(cFinal);
        
        if (m.reflectivity != 1.0f) {
            vec L_vector;
            subVec(&l.position, &XF.position, &L_vector);
            
            float L_length = mag(&L_vector);
            
            normalizeSelf(&L_vector);
            ray_shadow.direction = L_vector;
            ray_shadow.origin = XF.position;
            
            float first = traceShadow2(ray_shadow, XF.obj, scene, l, L_length) + m.reflectivity;
            
            first = fminf(1.0f, first);
            
            mulfvSelf(first, &cFinal);
        }
        
        
        
////        printVec(cFinal);
//        if (m.reflectivity != 1.0f) {
//            subVec(&l.position, &XF.position, &next_ray.direction);
////            normalize(&next_ray.direction, &next_ray.direction);
//            normalizeSelf(&next_ray.direction);
//            float first = traceShadow(next_ray, XF.obj, scene, l) + m.reflectivity;
////            mulfv(fminf(first, 1.0f), &cFinal, &cFinal);
//            mulfvSelf(fminf(first, 1.0f), &cFinal);
////            printVec(cFinal);
//        }
    }
    

    if (m.reflectivity > 0.0f) {
        Ray ray_refracted;
        ray_refracted.origin = XF.position;
        reflect2(&r.direction, &XF.normal, &ray_refracted.direction);
//        normalize(&next_ray.direction, &next_ray.direction);
        normalizeSelf(&ray_refracted.direction);
        vec temp1 = traceColor(ray_refracted, scene, n+1);
//        mulfv(m.reflectivity, &temp1, &temp1);
        mulfvSelf(m.reflectivity, &temp1);
//        addVec(&temp1, &cFinal, &cFinal);
        addVecSelf(&temp1, &cFinal);
//        printVec(cFinal);
    }
    
//    printVec(cFinal);

    
    if (m.refractivity > 0.0f) {
//        refract(&r.direction, &XF.normal, m.ior, dot(&r.direction, &XF.normal), &next_ray.direction);
//
////        normalize(&next_ray.direction, &next_ray.direction);
//        normalizeSelf(&next_ray.direction);
//
//        vec temp1 = traceColor(next_ray, scene, n+1);
//
////        mulfv(m.refractivity, &temp1, &temp1);
//        mulfvSelf(m.refractivity, &temp1);
//
////        addVec(&temp1, &cFinal, &cFinal);
//        addVecSelf(&temp1, &cFinal);
////        printVec(cFinal);
        Ray ray_refracted;
        
        vec N = XF.normal;
        float NdotI = dot(&r.direction, &N);
        float ior, n1, n2, cos_t;
        
        if (NdotI > 0.0f) {
            n1 = 1;
            n2 = m.ior;
            mulfv(-1, &N, &N);
        }
        else {
            n1 = m.ior;
            n2 = 1;
            NdotI = -NdotI;
        }
        
        ior = n2 / n1;
        cos_t = ior * ior * (1.0f - NdotI * NdotI);
        ray_refracted.origin = XF.position;
        refract2(&r.direction, &N, ior, NdotI, cos_t, &ray_refracted.direction);
        normalizeSelf(&ray_refracted.direction);
        vec temp1 = traceColor(ray_refracted, scene, n + 1);
        mulfvSelf(m.refractivity, &temp1);
        addVecSelf(&temp1, &cFinal);
    }
    
//    printVec(cFinal);

//    addVec(&m.ambient_color, &cFinal, &cFinal);
    addVecSelf(&m.ambient_color, &cFinal);
//    printVec(cFinal);
    
//    quickclampVector(&cFinal, &cFinal);
    quickclampVectorSelf(&cFinal);
//    printVec(cFinal);
    return cFinal;
}

vec traceColorSimple(Ray r, Scene* scene, int n) {

    vec cFinal = {0, 0, 0};
    Hit* xFinal = NULL;

    if (n > max_recursion) {
        return cFinal;
    }

    //find nearest object intersected by ray
    xFinal = basicLoop1(r, float_max, scene);

    //return black if no objects intersected
    if (xFinal == NULL) {
        return cFinal;
    }

    Ray next_ray;
    Hit XF;
    XF = *xFinal;

    next_ray.origin = XF.position;
    Material m = scene->materials[((Object*)(XF.obj))->mat];
    return m.ambient_color;
}

#endif /* Tracer_h */

