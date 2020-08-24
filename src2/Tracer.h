//
//  Tracer.h
//  SDL_ACCEL9
//
//  Created by Ben on 8/11/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Tracer_h
#define Tracer_h

#include <stdio.h>
#include "Camera.h"
#include "Scene.h"
#include "Settings.h"

#include <SDL2/SDL.h>
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
    update_camera(&tracer->camera, dt, tracer->keyboard_keys);
    update_Scene(&tracer->scene, dt);
}

void handle_key_pressed(SDL_Event event, SDL_bool val, Tracer* tracer) {
    // keyboard API for key pressed
    tracer->keyboard_keys[event.key.keysym.scancode] = val;
}

float traceShadow(Ray* ray, Object* thisobj, Scene* scene, Light* light, float L_length) {
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
        objf->interesect(ray, objf, L_length, &xFinal);
    } else {
        return 1.0f;
    }

    m = &scene->materials[objf->mat];

    vec tempcol;
    
    m->shading_func(&ray->direction, &xFinal.normal, &xFinal.position, light, &tempcol);
    float weight = averageVector(&tempcol);
    
    if (m->reflectivity > 0.0f) {
        weight -= m->reflectivity;
    }
    
    if (m->refractivity > 0.0f) {
        weight *= m->refractivity;
    }

    return weight;
}

void traceColor(Ray* ray, Scene* scene, float * restrict cFinal, int n) {
    Hit xFinal;
    
    if (n > max_recursion) {
        return;
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
        return;
    }
    else {
        objf->interesect(ray, objf, L_length, &xFinal);
    }

    Ray ray_shadow;

    Light* l;

    ray_shadow.origin = xFinal.position;
    Material* m = &scene->materials[objf->mat];

    for (int i = 0; i < scene->numLights; i++) {
        l = &scene->lights[i];
        
        m->shading_func(&ray->direction, &xFinal.normal, &xFinal.position, l, cFinal);
        
        if (m->reflectivity != 1.0f) {
            subVec(&l->position, &xFinal.position, &ray_shadow.direction);
                        
            normalizeSelf(&ray_shadow.direction);
            
            float first = traceShadow(&ray_shadow, objf, scene, l, mag(&ray_shadow.direction)) + m->reflectivity;
            
            first = fminf(1.0f, first);
            
            mulfvSelf(first, cFinal);
        }
    }
    
    vec temp1;
    
    if (m->reflectivity > 0.0f) {
        reflect(&ray->direction, &xFinal.normal, &ray_shadow.direction);
        normalizeSelf(&ray_shadow.direction);
        
        traceColor(&ray_shadow, scene, &temp1, n+1);
        
        mulfvSelf(m->reflectivity, &temp1);
        addVecSelf(&temp1, cFinal);
    }
    
    if (m->refractivity > 0.0f) {
        refract2(&ray->direction, &xFinal.normal, m->ior, &ray_shadow.direction);
        normalizeSelf(&ray_shadow.direction);
        
        traceColor(&ray_shadow, scene, &temp1, n + 1);
        
        mulfvSelf(m->refractivity, &temp1);
        addVecSelf(&temp1, cFinal);
    }
    
    addVecSelf(&m->ambient_color, cFinal);
    quickclampVectorSelf(cFinal);
}

#endif /* Tracer_h */

