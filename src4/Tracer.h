//
//  Tracer.h
//  SDL_ACCEL13
//
//  Created by Ben on 8/26/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Tracer_h
#define Tracer_h

#include <stdio.h>
#include "Camera.h"
#include "Scene.h"
#include "Settings.h"
#include "Object.h"
#include "InputHandler.h"

#include <SDL2/SDL.h>

#define float_max 99999999

typedef struct {
    Camera camera;
    Scene scene;
} Tracer;

Tracer newTracer(void (*initmode)(Scene* scene)) {
    Tracer tracer;
    tracer.camera = newCamera((simd_float3){0, 1, 0}, camera_speed, camera_sensitivity);
    tracer.scene = newScene(initmode);
    return tracer;
}

void update_Tracer(Tracer* tracer, double dt) {
    update_camera(&tracer->camera, dt, keyboard_keys);
    update_Scene(&tracer->scene, dt);
}

float traceShadow(const Ray ray, const Object* thisobj, const Scene* scene, const Light* light, float L_length) {
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
        xFinal.position = ray.direction * L_length + ray.origin;
        hit_normal;
    }
    else {
        return 1.0f;
    }
    
    m = &scene->materials[objf->mat];
    
    simd_float3 tempcol = m->shading_func(ray.direction, xFinal.normal, xFinal.position, light);
    
    float weight = (tempcol.x + tempcol.y + tempcol.z) / 3.0f;
    
    if (m->reflectivity > 0.0f) {
        weight -= m->reflectivity;
    }
    
    if (m->refractivity > 0.0f) {
        weight *= m->refractivity;
    }
    
    return weight;
}

simd_float3 traceColor(const Ray ray, const Scene* scene, const int n) {
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
        xFinal.position = ray.direction * L_length + ray.origin;
        hit_normal;
    }
    
    Ray ray_shadow;
    
    Light* l;
    
    ray_shadow.origin = xFinal.position;
    
    Material* m = &scene->materials[objf->mat];
    
    for (int i = 0; i < scene->numLights; i++) {
        l = &scene->lights[i];
        
        cFinal = m->shading_func(ray.direction, xFinal.normal, xFinal.position, l);
        
        if (m->reflectivity != 1.0f) {
            ray_shadow.direction = l->position - xFinal.position;
            float L_length = simd_length(ray_shadow.direction);
            
            ray_shadow.direction = simd_normalize(ray_shadow.direction);
            
            float first = traceShadow(ray_shadow,
                                      objf,
                                      scene,
                                      l,
                                      L_length) + m->reflectivity;
            
            first = fminf(1.0f, first);
            
            cFinal *= first;
            
        }
        
    }
    
    if (m->reflectivity > 0.0f) {
        ray_shadow.direction = simd_reflect(ray.direction, xFinal.normal);
        
        ray_shadow.direction = simd_normalize(ray_shadow.direction);
                
        cFinal += traceColor(ray_shadow, scene, n+1) * m->reflectivity;
    }
    
    if (m->refractivity > 0.0f) {
        float NdotI, cos_t, ior;
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
            ray_shadow.direction = (ior*NdotI-cos_t) * xFinal.normal + ior * ray.direction;
        }
        else {
            ray_shadow.direction = simd_reflect(ray.direction, xFinal.normal);
        }
        
        ray_shadow.direction = simd_normalize(ray_shadow.direction);
                
        cFinal += traceColor(ray_shadow, scene, n+1) * m->refractivity;
    }
    
    return simd_clamp(cFinal + m->ambient_color, 0, 1);
}

#endif /* Tracer_h */

