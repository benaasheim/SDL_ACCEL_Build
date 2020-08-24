//
//  Materials.h
//  SDL_ACCEL9
//
//  Created by Ben on 8/11/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Materials_h
#define Materials_h

#include <stdio.h>
#include "Vector.h"
#include "Light.h"
#include "Hit.h"
#include "Settings.h"
#include "Ray.h"

//typedef struct Material {
//    vec ambient_color;
//    float roughness;
//    float fresnel;
//    float density;
//    float reflectivity;
//    float refractivity;
//    float ior;
////    void (*shading_func)(Ray r, Hit x, Light l, float * restrict a);
//    void (*shading_func)(float * restrict raydir,
//    float * restrict N,
//    float * restrict P,
//    Light* l,
//    float * restrict a)
//} Material;

struct __attribute__ ((__packed__)) Material {
    vec ambient_color;
    float roughness;
    float fresnel;
    float density;
    float reflectivity;
    float refractivity;
    float ior;
//    void (*shading_func)(Ray r, Hit x, Light l, float * restrict a);
    void (*shading_func)(float * restrict raydir,
    float * restrict N,
    float * restrict P,
    Light* l,
    float * restrict a)
};

typedef struct Material Material;

Material newMaterial(vec ambient_color, vec diffuse_color, vec specular_color, float roughness, float fresnel, float density, float reflectivity, float refractivity, float ior,
void (*shading_func)(float * restrict raydir,
float * restrict N,
float * restrict P,
Light* l,
float * restrict a)
                     ) {
    Material material;
    material.ambient_color = ambient_color;
    material.roughness = roughness;
    material.fresnel = fresnel;
    material.density = density;
    material.reflectivity = reflectivity;
    material.refractivity = refractivity;
    material.ior = ior;
    material.shading_func = shading_func;
    return material;
}

void shade_Diffuse_White_Fast(float * restrict raydir,
                              float * restrict N,
                              float * restrict P,
                              Light* l,
                              float * restrict a) {
    
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
    vec L, V, H;
    
    subVec(&l->position, P, &L);
    L_length = mag(&L);
    normalizeSelf(&L);
    mulfv(-1.0f, raydir, &V);
    addVec(&V, &L, &H);
    normalize(&H, &H);
    
    A = l->constant + l->linear * L_length + l->exponent * L_length * L_length + epsilon;
    
    //Calculate the dot products required for shading
    NdotL = fminf(dot(N, &L), 1.0f);
    NdotV = fminf(dot(N, &V), 1.0f);
    NdotH = fminf(dot(N, &H), 1.0f);
    VdotH = fminf(dot(&V, &H), 1.0f);
    
    
    

    
    if (NdotL < epsilon) {
        mulfvSelf(0, a);
        return;
    }
    
//    float F
////    = m.fresnel;
//     = 0.5f;
//    float K
////    = m.density;
//     = 0.9f;
    
    // Evaluate the geometric term
    float f1 = fminf(NdotV, NdotL) * 2.0f * NdotH / VdotH;
    lambertian = fminf(1.0f, f1);
        
    // Evaluate the roughness term
    float ndh = NdotH * NdotH;
    float roughness_a = 1.0f / (0.5625 * (float) powf(NdotH, 4));
    float roughness_b = ndh - 1.0f;
    float roughness_c = 0.140625 * ndh;
    roughness = roughness_a * expf(roughness_b/roughness_c);
        
    // Evaluate the fresnel term
    specular = powf(1.0f - VdotH, 5);
    specular *= 1.0f - 0.5f;
    specular += 0.5f;
        
    // Put all the terms together
    float Rs_numerator = lambertian * roughness * specular;
    float Rs_denominator = fmaxf(NdotV * NdotL, epsilon);
    float Rs = Rs_numerator / Rs_denominator;
        
    // Add all the terms to C
    float tfloat = Rs * 0.1f + 0.9f;
    mulfv(NdotL * tfloat / A, &l->color, a);
}

void shade_Diffuse_White_Faster(float * restrict raydir,
                                float * restrict N,
                                float * restrict P,
                                Light* l,
                                float * restrict a) {
    
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
    vec L, V, H;
    
    subVec(&l->position, P, &L);
    L_length = mag(&L);
    normalizeSelf(&L);
//    mulfv(-1.0f, raydir, &V);
    negateVec(raydir, &V);
    addVec(&V, &L, &H);
    normalizeSelf(&H);
    
    A = l->constant + l->linear * L_length + l->exponent * L_length * L_length + epsilon;
    
    //Calculate the dot products required for shading
    NdotL = fminf(dot(N, &L), 1.0f);
    NdotV = fminf(dot(N, &V), 1.0f);
    NdotH = fminf(dot(N, &H), 1.0f);
    VdotH = fminf(dot(&V, &H), 1.0f);
    
    
    

    
    if (NdotL < epsilon) {
//        mulfvSelf(0, a);
        zeroV(a);
        return;
    }
    
    // Evaluate the geometric term
    float f1 = fminf(NdotV, NdotL) * 2.0f * NdotH / VdotH;
    lambertian = fminf(1.0f, f1);
        
    // Evaluate the roughness term
    float ndh = NdotH * NdotH;
    float roughness_a = 1.0f / (0.5625 * (float) powf(NdotH, 4));
    float roughness_b = ndh - 1.0f;
    float roughness_c = 0.140625 * ndh;
    roughness = roughness_a * expf(roughness_b/roughness_c);
        
    // Evaluate the fresnel term
    specular = powf(1.0f - VdotH, 5);
    specular *= 1.0f - 0.5f;
    specular += 0.5f;
        
    // Put all the terms together
    float Rs_numerator = lambertian * roughness * specular;
    float Rs_denominator = fmaxf(NdotV * NdotL, epsilon);
    float Rs = Rs_numerator / Rs_denominator;
        
    // Add all the terms to C
//    float tfloat = Rs * 0.1f + 0.9f;
    mulfv(NdotL * (Rs * 0.1f + 0.9f) / A, &l->color, a);
}

Material Diffuse_White() {
    vec ac = {0.01f, 0.01f, 0.01f};
    vec dc = {1, 1, 1};
    vec sc = {1, 1, 1};
    float roughness = 0.375;
    float fresnel = 0.5f;
    float density = 0.9f;
    float reflectivity = 0.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, roughness, fresnel, density, reflectivity, refractivity, ior, shade_Diffuse_White_Faster);
}

void shade_Diffuse_Red_Fast(float * restrict raydir,
                            float * restrict N,
                            float * restrict P,
                            Light* l,
                            float * restrict a) {
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
    vec L, V, H;
    
    subVec(&l->position, P, &L);
    L_length = mag(&L);
    normalizeSelf(&L);
    mulfv(-1.0f, raydir, &V);
    addVec(&V, &L, &H);
    normalize(&H, &H);
    
    A = l->constant + l->linear * L_length + l->exponent * L_length * L_length + epsilon;
    
    //Calculate the dot products required for shading
    NdotL = fminf(dot(N, &L), 1.0f);
    NdotV = fminf(dot(N, &V), 1.0f);
    NdotH = fminf(dot(N, &H), 1.0f);
    VdotH = fminf(dot(&V, &H), 1.0f);
    
    if (NdotL < epsilon) {
        mulfvSelf(0, a); return;
    }
    
    float R
    //    = m.roughness;
        = 0.375;
        float F
    //    = m.fresnel;
         = 0.5f;
        float K
    //    = m.density;
         = 0.9f;
        
    // Evaluate the geometric term
    float geo_numerator = 2.0f * NdotH;
    float geo_a = (geo_numerator * NdotV) / VdotH;
    float geo_b = (geo_numerator * NdotL) / VdotH;
    float f1 = fminf(geo_a, geo_b);
    lambertian = fminf(1.0f, f1);
        
    // Evaluate the roughness term
    float roughness_a = 1.0f / (4.0f * R * R * (float) powf(NdotH, 4));
    float roughness_b = NdotH * NdotH - 1.0f;
    float roughness_c = R * R * NdotH * NdotH;
    roughness = roughness_a * expf(roughness_b/roughness_c);
        
    // Evaluate the fresnel term
    specular = powf(1.0f - VdotH, 5);
    specular *= 1.0f - F;
    specular += F;
        
    // Put all the terms together
    float Rs_numerator = lambertian * roughness * specular;
    float Rs_denominator = fmaxf(NdotV * NdotL, epsilon);
    float Rs = Rs_numerator / Rs_denominator;
        
    // Add all the terms to C
    mulfv(NdotL / A, &l->color, a);

    float tfloat = Rs * (1.0f - K);

    vec final_b = {tfloat+K, tfloat, tfloat};

    mulvv(&final_b, a, a);
}

Material Diffuse_Red() {
    vec ac = {0.01f, 0.01f, 0.01f};
    vec dc = {1, 0, 0};
    vec sc = {1, 1, 1};
    float roughness = 0.375;
    float fresnel = 0.5f;
    float density = 0.9f;
    float reflectivity = 0.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, roughness, fresnel, density, reflectivity, refractivity, ior, shade_Diffuse_Red_Fast);
}

void shade_Diffuse_Blue_Fast(float * restrict raydir,
                             float * restrict N,
                             float * restrict P,
                             Light* l,
                             float * restrict a
                             ) {
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
    vec L, V, H;
    
    subVec(&l->position, P, &L);
    L_length = mag(&L);
    normalizeSelf(&L);
    mulfv(-1.0f, raydir, &V);
    addVec(&V, &L, &H);
    normalize(&H, &H);
    
    A = l->constant + l->linear * L_length + l->exponent * L_length * L_length + epsilon;
    
    //Calculate the dot products required for shading
    NdotL = fminf(dot(N, &L), 1.0f);
    NdotV = fminf(dot(N, &V), 1.0f);
    NdotH = fminf(dot(N, &H), 1.0f);
    VdotH = fminf(dot(&V, &H), 1.0f);
        
    if (NdotL < epsilon) {
        mulfvSelf(0, a); return;
    }
        
        float R
    //    = m.roughness;
        = 0.375;
        float F
    //    = m.fresnel;
         = 0.5f;
        float K
    //    = m.density;
         = 0.9f;
        
    // Evaluate the geometric term
    float geo_numerator = 2.0f * NdotH;
    float geo_a = (geo_numerator * NdotV) / VdotH;
    float geo_b = (geo_numerator * NdotL) / VdotH;
    float f1 = fminf(geo_a, geo_b);
    lambertian = fminf(1.0f, f1);
        
    // Evaluate the roughness term
    float roughness_a = 1.0f / (4.0f * R * R * (float) powf(NdotH, 4));
    float roughness_b = NdotH * NdotH - 1.0f;
    float roughness_c = R * R * NdotH * NdotH;
    roughness = roughness_a * expf(roughness_b/roughness_c);
        
    // Evaluate the fresnel term
    specular = powf(1.0f - VdotH, 5);
    specular *= 1.0f - F;
    specular += F;
        
    // Put all the terms together
    float Rs_numerator = lambertian * roughness * specular;
    float Rs_denominator = fmaxf(NdotV * NdotL, epsilon);
    float Rs = Rs_numerator / Rs_denominator;
        
    // Add all the terms to C
    mulfv(NdotL / A, &l->color, a);

    float tfloat = Rs * (1.0f - K);

    vec final_b = {tfloat, tfloat, tfloat+K};

    mulvv(&final_b, a, a);
}

Material Diffuse_Blue() {
    vec ac = {0.01f, 0.01f, 0.01f};
    vec dc = {0, 0, 1};
    vec sc = {1, 1, 1};
    float roughness = 0.375;
    float fresnel = 0.5f;
    float density = 0.9f;
    float reflectivity = 0.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, roughness, fresnel, density, reflectivity, refractivity, ior, shade_Diffuse_Blue_Fast);
}

void shade_Reflective_Metal_Fast(float * restrict raydir,
                                 float * restrict N,
                                 float * restrict P,
                                 Light* l,
                                 float * restrict a
                                 )
{
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
    vec L, V, H;
    
    subVec(&l->position, P, &L);
    L_length = mag(&L);
    normalizeSelf(&L);
    mulfv(-1.0f, raydir, &V);
    addVec(&V, &L, &H);
    normalize(&H, &H);
    
    A = l->constant + l->linear * L_length + l->exponent * L_length * L_length + epsilon;
    
    //Calculate the dot products required for shading
    NdotL = fminf(dot(N, &L), 1.0f);
    NdotV = fminf(dot(N, &V), 1.0f);
    NdotH = fminf(dot(N, &H), 1.0f);
    VdotH = fminf(dot(&V, &H), 1.0f);
    
    if (NdotL < epsilon) {
        mulfvSelf(0, a); return;
    }
        
    float R
    //    = m.roughness;
    = 0.1F;
    float F
    //    = m.fresnel;
    = 1.0f;
    float K
    //    = m.density;
    = 0.5f;
        
    // Evaluate the geometric term
    float geo_numerator = 2.0f * NdotH;
    float geo_a = (geo_numerator * NdotV) / VdotH;
    float geo_b = (geo_numerator * NdotL) / VdotH;
    float f1 = fminf(geo_a, geo_b);
    lambertian = fminf(1.0f, f1);
        
    // Evaluate the roughness term
    float roughness_a = 1.0f / (4.0f * R * R * (float) powf(NdotH, 4));
    float roughness_b = NdotH * NdotH - 1.0f;
    float roughness_c = R * R * NdotH * NdotH;
    roughness = roughness_a * expf(roughness_b/roughness_c);
        
    // Evaluate the fresnel term
    specular = powf(1.0f - VdotH, 5);
    specular *= 1.0f - F;
    specular += F;
        
    // Put all the terms together
    float Rs_numerator = lambertian * roughness * specular;
    float Rs_denominator = fmaxf(NdotV * NdotL, epsilon);
    float Rs = Rs_numerator / Rs_denominator;
        
    // Add all the terms to C
    float tfloat = Rs * (1.0f - K);

    mulfv(NdotL * tfloat / A, &l->color, a);
}

Material Reflective_Metal() {
    vec ac = {0.01f, 0.01f, 0.01f};
    vec dc = {0, 0, 0};
    vec sc = {1, 1, 1};
    float roughness = 0.1f;
    float fresnel = 1.0f;
    float density = 0.5f;
    float reflectivity = 1.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, roughness, fresnel, density, reflectivity, refractivity, ior, shade_Reflective_Metal_Fast);
}

void shade_Refractive_Glass_Fast(float * restrict raydir,
                                 float * restrict N,
                                 float * restrict P,
                                 Light* l,
                                 float * restrict a
                                 ) {
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
    vec L, V, H;
    
    subVec(&l->position, P, &L);
    L_length = mag(&L);
    normalizeSelf(&L);
    mulfv(-1.0f, raydir, &V);
    addVec(&V, &L, &H);
    normalize(&H, &H);
    
    A = l->constant + l->linear * L_length + l->exponent * L_length * L_length + epsilon;
    
    //Calculate the dot products required for shading
    NdotL = fminf(dot(N, &L), 1.0f);
    NdotV = fminf(dot(N, &V), 1.0f);
    NdotH = fminf(dot(N, &H), 1.0f);
    VdotH = fminf(dot(&V, &H), 1.0f);
    
    if (NdotL < epsilon) {
        mulfvSelf(0, a); return;
    }
    
    float R
    //    = m.roughness;
    = 0.1F;
    float F
    //    = m.fresnel;
    = 1.0f;
    float K
    //    = m.density;
    = 0.5f;
    
    // Evaluate the geometric term
    float geo_numerator = 2.0f * NdotH;
    float geo_a = (geo_numerator * NdotV) / VdotH;
    float geo_b = (geo_numerator * NdotL) / VdotH;
    float f1 = fminf(geo_a, geo_b);
    lambertian = fminf(1.0f, f1);
        
    // Evaluate the roughness term
    float roughness_a = 1.0f / (4.0f * R * R * (float) powf(NdotH, 4));
    float roughness_b = NdotH * NdotH - 1.0f;
    float roughness_c = R * R * NdotH * NdotH;
    roughness = roughness_a * expf(roughness_b/roughness_c);
        
    // Evaluate the fresnel term
    specular = powf(1.0f - VdotH, 5);
    specular *= 1.0f - F;
    specular += F;
        
    // Put all the terms together
    float Rs_numerator = lambertian * roughness * specular;
    float Rs_denominator = fmaxf(NdotV * NdotL, epsilon);
    float Rs = Rs_numerator / Rs_denominator;
        
    // Add all the terms to C
    float tfloat = Rs * (1.0f - K);
    
    mulfv(NdotL * tfloat / A, &l->color, a);
}

Material Refractive_Glass() {
    vec ac = {0.01f, 0.01f, 0.01f};
    vec dc = {0, 0, 0};
    vec sc = {1, 1, 1};
    float roughness = 0.1f;
    float fresnel = 1.0f;
    float density = 0.5f;
    float reflectivity = 0.0f;
    float refractivity = 1.0f;
    float ior = 2.0f;
    return newMaterial(ac, dc, sc, roughness, fresnel, density, reflectivity, refractivity, ior, shade_Refractive_Glass_Fast);
}

void shade_Diffuse_Sky_Blue_Fast(float * restrict raydir,
                                 float * restrict N,
                                 float * restrict P,
                                 Light* l,
                                 float * restrict a
                                 ) {
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
    vec C, L, V, H;
    
    subVec(&l->position, P, &L);
    L_length = mag(&L);
    normalizeSelf(&L);
    mulfv(-1.0f, raydir, &V);
    addVec(&V, &L, &H);
    normalize(&H, &H);
    
    A = l->constant + l->linear * L_length + l->exponent * L_length * L_length + epsilon;
    
    //Calculate the dot products required for shading
    NdotL = fminf(dot(N, &L), 1.0f);
    NdotV = fminf(dot(N, &V), 1.0f);
    NdotH = fminf(dot(N, &H), 1.0f);
    VdotH = fminf(dot(&V, &H), 1.0f);
    
        
    if (NdotL < epsilon) {
        mulfvSelf(0, a); return;
    }
        
        float R
    //    = m.roughness;
        = 0.375;
        float F
    //    = m.fresnel;
         = 0.5f;
        float K
    //    = m.density;
         = 0.9f;
        
    // Evaluate the geometric term
    float geo_numerator = 2.0f * NdotH;
    float geo_a = (geo_numerator * NdotV) / VdotH;
    float geo_b = (geo_numerator * NdotL) / VdotH;
    float f1 = fminf(geo_a, geo_b);
    lambertian = fminf(1.0f, f1);
        
    // Evaluate the roughness term
    float roughness_a = 1.0f / (4.0f * R * R * (float) powf(NdotH, 4));
    float roughness_b = NdotH * NdotH - 1.0f;
    float roughness_c = R * R * NdotH * NdotH;
    roughness = roughness_a * expf(roughness_b/roughness_c);
        
    // Evaluate the fresnel term
    specular = powf(1.0f - VdotH, 5);
    specular *= 1.0f - F;
    specular += F;
        
    // Put all the terms together
    float Rs_numerator = lambertian * roughness * specular;
    float Rs_denominator = fmaxf(NdotV * NdotL, epsilon);
    float Rs = Rs_numerator / Rs_denominator;
        
    // Add all the terms to C
    vec final_a;
    
    mulfv(NdotL, &l->color, &final_a);

    vec final_b;
        
    vec temp2;
    vec dc = {0.2f, 0.2f, 0.9f};
    mulfv(K, &dc, &temp2);
        
    vec temp3;
    
    float tfloat = Rs * (1.0f - K);
    
    vec sc = {1, 1, 1};
    
    mulfv(tfloat, &sc, &temp3);
        
    addVec(&temp2, &temp3, &final_b);
    
    mulvv(&final_a, &final_b, &C);
    
    divideFloat(&C, A, a);
}

Material Diffuse_Sky_Blue() {
    vec ac = {0.2f, 0.25f, 0.75f};
    vec dc = {0.2f, 0.2f, 0.9f};
    vec sc = {1, 1, 1};
    float roughness = 0.9f;
    float fresnel = 0.5f;
    float density = 0.9f;
    float reflectivity = 0.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, roughness, fresnel, density, reflectivity, refractivity, ior, shade_Diffuse_Sky_Blue_Fast);
}

#endif /* Materials_h */

