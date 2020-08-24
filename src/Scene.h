//
//  Scene.h
//  SDL_ACCEL6
//
//  Created by Ben on 8/6/20.
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

typedef struct {
    // Scene information
    unsigned int numObjects;
    Object objects[max_objects];
    unsigned int numLights;
    Light lights[max_lights];
    unsigned int numMaterials;
    Material materials[max_materials];
} Scene;

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

Material* diffWhite(Scene* scene) {
    return &scene->materials[0];
}

Material* diffRed(Scene* scene) {
    return &scene->materials[1];
}

Material* diffBlue(Scene* scene) {
    return &scene->materials[2];
}

Material* reflMetal(Scene* scene) {
    return &scene->materials[3];
}
Material* refrGlass(Scene* scene) {
    return &scene->materials[4];
}

void init_Scene1(Scene* scene) {
    //Lights
    vec lcvec = {1.0f, 1.0f, 1.0f};
    vec lpvec = {0.0f, 4.0f, -5.0f};
    add_Light(scene, newLight(lcvec, lpvec, 1.0f, 0.0f, 0.0f, 0.1f));
    
    //Materials
    add_Material(scene, Diffuse_White());
    add_Material(scene, Diffuse_Red());
    add_Material(scene, Diffuse_Blue());
    add_Material(scene, Reflective_Metal());
    add_Material(scene, Refractive_Glass());
    
    //Objects
    vec svec1 = {2, 0.75, -2};
    vec svec2 = {0, 0.75, -3};
    vec svec3 = {-2, 0.75, -4};
    add_Object(scene, makeSphere(svec1, 0.75, 3));
    add_Object(scene, makeSphere(svec2, 0.25, 4));
    add_Object(scene, makeSphere(svec3, 0.25, 4));
    vec pvec1 = {0, 0, 0};
    vec pvec2 = {0, 5, 0};
    vec pvec3 = {0, 0, -10};
    vec pvec4 = {4, 0, 0};
    vec pvec5 = {-4, 0, 0};
    vec pvec11 = {0, 1, 0};
    vec pvec12 = {0, -1, 0};
    vec pvec13 = {0, 0, 1};
    vec pvec14 = {-1, 0, 0};
    vec svec15 = {1, 0, 0};
    add_Object(scene, makePlane(pvec1, pvec11, 0));
    add_Object(scene, makePlane(pvec2, pvec12, 0));
    add_Object(scene, makePlane(pvec3, pvec13, 0));
    add_Object(scene, makePlane(pvec4, pvec14, 1));
    add_Object(scene, makePlane(pvec5, svec15, 2));
}

void init_Scene2(Scene* scene) {
    //Lights
    vec scvec = {1.0f, 0.7f, 0.5f};
    vec spvec = {2.0f, 450.0f, -24.0f};
    add_Sun(scene, newSun(scvec, spvec, 100000.0f, 0.0f, 0.0f, 0.9f, 0.25f, 4));
    
    //Materials
    add_Material(scene, Diffuse_White());
    add_Material(scene, Diffuse_Red());
    add_Material(scene, Diffuse_Blue());
    add_Material(scene, Reflective_Metal());
    add_Material(scene, Refractive_Glass());
    add_Material(scene, Diffuse_Sky_Blue());
    
    //Objects
    vec svec1 = {2, 0.75, -2};
    vec svec2 = {0, 0.75, -3};
    vec svec3 = {-2, 0.75, -4};
    add_Object(scene, makeSphere(svec1, 0.75, 3));
    add_Object(scene, makeSphere(svec2, 0.25, 4));
    add_Object(scene, makeSphere(svec3, 0.25, 4));
    vec pvec1 = {0, 0, 0};
    vec pvec2 = {0, 500, 0};
    vec pvec11 = {0, 1, 0};
    vec pvec12 = {0, -1, 0};
    add_Object(scene, makePlane(pvec1, pvec11, 0));
    add_Object(scene, makePlane(pvec2, pvec12, 5));
}

void update_Scene(Scene* scene, float dt) {
    
}

#endif /* Scene_h */

