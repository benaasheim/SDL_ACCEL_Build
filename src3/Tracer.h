//
//  Tracer.h
//  SDL_ACCEL10
//
//  Created by Ben on 8/13/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Tracer_h
#define Tracer_h

#include <stdio.h>
#include "Camera.h"
#include "Scene.h"
#include "Settings.h"

#include <SDL2/SDL.h>

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
    simd_float3 cvec = {0, 1, 0};
    tracer.camera = newCamera(cvec, camera_speed, camera_sensitivity);
    tracer.scene = newScene(initmode);
    for (int i = 0; i < 255; i++) {
        tracer.keyboard_keys[i] = SDL_FALSE;
    }
    return tracer;
}

void update_Tracer(Tracer* tracer, double dt) {
    update_camera(&tracer->camera, dt, tracer->keyboard_keys);
    update_Scene(&tracer->scene, dt);
}

void handle_key_pressed(SDL_Event event, SDL_bool val, Tracer* tracer) {
    // keyboard API for key pressed
    tracer->keyboard_keys[event.key.keysym.scancode] = val;
}

float traceShadow(Ray ray, Object* thisobj, Scene* scene, Light* light, float L_length) {
    Hit xFinal;
    Material* m;
    
    //find nearest object intersected by ray
    Object* obj;
    Object* objf = NULL;
    float t;
    
    for (int i = 0; i < scene->numObjects; i++) {
        obj = &scene->objects[i];

        if (thisobj != obj) {
            t = obj->gett(ray, obj);

            if (t > 0.0f && t < L_length) {
                L_length = t;
                objf = obj;
            }
        }
    }
    
    if (objf != NULL) {
        xFinal = objf->interesect(ray, objf, L_length);
    }
    else {
        return 1.0f;
    }
    
    m = &scene->materials[objf->mat];
    
    simd_float3 tempcol
    
//    = m->shading_func(ray, xFinal, *light, *m)
    = m->shading_func(ray.direction, xFinal.normal, xFinal.position, light)
    ;
    
    float weight = (tempcol.x + tempcol.y + tempcol.z) / 3.0f;
    
    if (m->reflectivity > 0.0f) {
        weight -= m->reflectivity;
    }
    
    if (m->refractivity > 0.0f) {
        weight *= m->refractivity;
    }
    
    return weight;
}

simd_float3 traceColor(Ray ray, Scene* scene, int n) {
    Hit xFinal;
    simd_float3 cFinal = {0, 0, 0};
    
    if (n > max_recursion) {
        return cFinal;
    }
    
    //find nearest object intersected by ray
    float L_length = float_max;
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
    
    if (objf == NULL) {
        return cFinal;
    }
    else {
        xFinal = objf->interesect(ray, objf, L_length);
    }
    
    Ray ray_shadow;
    
    Light* l;
    
    ray_shadow.origin = xFinal.position;
    
    Material* m = &scene->materials[objf->mat];
    
    for (int i = 0; i < scene->numLights; i++) {
        l = &scene->lights[i];
        
        cFinal = m->shading_func(ray.direction, xFinal.normal, xFinal.position, l);
//        cFinal = m->shading_func(ray, xFinal, *l, *m);
        
        if (m->reflectivity != 1.0f) {
            ray_shadow.direction = l->position - xFinal.position;
            float L_length = simd_length(ray_shadow.direction);
            
            ray_shadow.direction = simd_normalize(ray_shadow.direction);
            
//            float first = traceShadow(&ray_shadow, objf, scene, l, mag(&ray_shadow.direction)) + m->reflectivity;
            
            float first = traceShadow(ray_shadow,
                                      objf,
                                      scene,
                                      l,
                                      L_length) + m->reflectivity;
            
            first = fminf(1.0f, first);
            
            cFinal *= first;
            
        }
        
    }
    
    
    simd_float3 temp1;
    
    if (m->reflectivity > 0.0f) {
        ray_shadow.direction = simd_reflect(ray.direction, xFinal.normal);
        
        ray_shadow.direction = simd_normalize(ray_shadow.direction);
        
        temp1 = traceColor(ray_shadow, scene, n+1);
        
        cFinal += temp1 * m->reflectivity;
    }
    
    if (m->refractivity > 0.0f) {
        float NdotI, cos_t, tfloat, ior;
        ior = m->ior;
        NdotI = simd_dot(ray.direction, xFinal.normal);
        if (NdotI > 0) {
            xFinal.normal *= -1;
        }
        else {
            ior = 1/ior;
            NdotI = -NdotI;
        }
        cos_t = ior * ior * (1.0f - NdotI * NdotI);
        cos_t = sqrtf(1.0f - cos_t);
        if (cos_t < 1.0f) {
            tfloat = ior*NdotI-cos_t;
            ray_shadow.direction = tfloat * xFinal.normal + ior * ray.direction;
        }
        else {
            ray_shadow.direction = simd_reflect(ray.direction, xFinal.normal);
        }
//        ray_shadow.direction = simd_refract(ray.direction, xFinal.normal, m->ior);
        
        ray_shadow.direction = simd_normalize(ray_shadow.direction);
        
        temp1 = traceColor(ray_shadow, scene, n+1);
        
        cFinal += temp1 * m->refractivity;
    }
    
    cFinal += m->ambient_color;
        
    cFinal = simd_clamp(cFinal, 0, 1);
    
    return  cFinal;
}

#endif /* Tracer_h */

