//
//  Scene.h
//  SDL_ACCEL10
//
//  Created by Ben on 8/13/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Scene_h
#define Scene_h


#include <stdio.h>
#define max_lights 50
#define max_objects 100
#define max_materials 50

#include "Object.h"
#include "Light.h"
#include "Sun.h"

struct __attribute__ ((__packed__)) Scene {
    // Scene information
    unsigned int numObjects;
    Object objects[max_objects];
    unsigned int numLights;
    Light lights[max_lights];
    unsigned int numMaterials;
    Material materials[max_materials];
};

typedef struct Scene Scene;

Scene newScene(void (*initmode)(Scene* scene)) {
    Scene scene;
    scene.numObjects = 0;
    scene.numLights = 0;
    scene.numMaterials = 0;
    initmode(&scene);
    return scene;
}

void add_Light(Scene* scene, Light light) {
    if (scene->numLights < max_lights) {
        scene->lights[scene->numLights++] = light;
    }
}

void add_Object(Scene* scene, Object object) {
    if (scene->numObjects < max_objects) {
        scene->objects[scene->numObjects++] = object;
    }
}

void add_Material(Scene* scene, Material material) {
    if (scene->numMaterials < max_materials) {
        scene->materials[scene->numMaterials++] = material;
    }
}

void add_Sun(Scene* scene, Sun sun) {
//    add_Object(scene, sun.sphere);
    add_Light(scene, sun.light);
}

void init_Scene1(Scene* scene) {
    //Lights
    simd_float3 lcvec = {1.0f, 1.0f, 1.0f};
    simd_float3 lpvec = {0.0f, 4.0f, -5.0f};
    add_Light(scene, newLight(lcvec, lpvec, 1.0f, 0.0f, 0.0f, 0.1f));
    
    //Materials
    add_Material(scene, Diffuse_White());
    add_Material(scene, Diffuse_Red());
    add_Material(scene, Diffuse_Blue());
    add_Material(scene, Reflective_Metal());
    add_Material(scene, Refractive_Glass());
    
    //Objects
    simd_float3 svec1 = {2, 0.75, -2};
    simd_float3 svec2 = {0, 0.75, -3};
    simd_float3 svec3 = {-2, 0.75, -4};
    add_Object(scene, makeSphere(svec1, 0.75, 3));
    add_Object(scene, makeSphere(svec2, 0.25, 4));
    add_Object(scene, makeSphere(svec3, 0.25, 4));
    simd_float3 pvec1 = {0, 0, 0};
    simd_float3 pvec2 = {0, 5, 0};
    simd_float3 pvec3 = {0, 0, -10};
    simd_float3 pvec4 = {4, 0, 0};
    simd_float3 pvec5 = {-4, 0, 0};
    simd_float3 pvec11 = {0, 1, 0};
    simd_float3 pvec12 = {0, -1, 0};
    simd_float3 pvec13 = {0, 0, 1};
    simd_float3 pvec14 = {-1, 0, 0};
    simd_float3 svec15 = {1, 0, 0};
    add_Object(scene, makePlane(pvec1, pvec11, 0));
    add_Object(scene, makePlane(pvec2, pvec12, 0));
    add_Object(scene, makePlane(pvec3, pvec13, 0));
    add_Object(scene, makePlane(pvec4, pvec14, 1));
    add_Object(scene, makePlane(pvec5, svec15, 2));
}

void init_Scene2(Scene* scene) {
    //Lights
    simd_float3 scvec = {1.0f, 0.7f, 0.5f};
    simd_float3 spvec = {2.0f, 450.0f, -24.0f};
    add_Sun(scene, newSun(scvec, spvec, 100000.0f, 0.0f, 0.0f, 0.9f, 0.25f, 4));
    
    //Materials
    add_Material(scene, Diffuse_White());
    add_Material(scene, Diffuse_Red());
    add_Material(scene, Diffuse_Blue());
    add_Material(scene, Reflective_Metal());
    add_Material(scene, Refractive_Glass());
    add_Material(scene, Diffuse_Sky_Blue());
    
    //Objects
    simd_float3 svec1 = {2, 0.75, -2};
    simd_float3 svec2 = {0, 0.75, -3};
    simd_float3 svec3 = {-2, 0.75, -4};
    add_Object(scene, makeSphere(svec1, 0.75, 3));
    add_Object(scene, makeSphere(svec2, 0.25, 4));
    add_Object(scene, makeSphere(svec3, 0.25, 4));
    simd_float3 pvec1 = {0, 0, 0};
    simd_float3 pvec2 = {0, 500, 0};
    simd_float3 pvec11 = {0, 1, 0};
    simd_float3 pvec12 = {0, -1, 0};
    add_Object(scene, makePlane(pvec1, pvec11, 0));
    add_Object(scene, makePlane(pvec2, pvec12, 5));
}

void init_Scene3(Scene* scene) {
    //Lights
    simd_float3 lcvec = {1.0f, 1.0f, 1.0f};
    simd_float3 lpvec = {0.0f, 4.0f, -5.0f};
    add_Light(scene, newLight(lcvec, lpvec, 1.0f, 0.0f, 0.0f, 0.1f));
    
    //Materials
    add_Material(scene, Diffuse_White());
    add_Material(scene, Diffuse_Red());
    add_Material(scene, Diffuse_Blue());
    add_Material(scene, Reflective_Metal());
    add_Material(scene, Refractive_Glass());
    
    //Objects
    simd_float3 svec1 = {2, 0.75, -2};
    simd_float3 svec2 = {0, 0.75, -3};
    simd_float3 svec3 = {-2, 0.75, -4};
    add_Object(scene, makeSphere(svec1, 0.75, 3));
    add_Object(scene, makeSphere(svec2, 0.25, 4));
    add_Object(scene, makeSphere(svec3, 0.25, 4));
    simd_float3 pvec1 = {0, 0, 0};
    simd_float3 pvec2 = {0, 5, 0};
    simd_float3 pvec3 = {0, 0, -10};
    simd_float3 pvec4 = {4, 0, 0};
    simd_float3 pvec5 = {-4, 0, 0};
    simd_float3 pvec11 = {0, 1, 0};
    simd_float3 pvec12 = {0, -1, 0};
    simd_float3 pvec13 = {0, 0, 1};
    simd_float3 pvec14 = {-1, 0, 0};
    simd_float3 svec15 = {1, 0, 0};
    add_Object(scene, makePlane(pvec1, pvec11, 0));
    add_Object(scene, makePlane(pvec2, pvec12, 0));
    add_Object(scene, makePlane(pvec3, pvec13, 0));
    add_Object(scene, makePlane(pvec4, pvec14, 1));
    add_Object(scene, makePlane(pvec5, svec15, 2));
    
    simd_float3 t1a = {1, 1, -7};
    simd_float3 t1b = {0, 4.5, -7};
    simd_float3 t1c = {-1, 1, -7};
    add_Object(scene, makeTriangle(t1a, t1b, t1c, 2));
}

void update_Scene(Scene* scene, float dt) {
    
}

#endif /* Scene_h */

