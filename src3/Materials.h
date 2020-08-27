//
//  Materials.h
//  SDL_ACCEL10
//
//  Created by Ben on 8/13/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Materials_h
#define Materials_h

#include <stdio.h>
#include "Light.h"
#include "Hit.h"
#include "Settings.h"
#include "Ray.h"

struct __attribute__ ((__packed__)) Material {
    simd_float3 ambient_color;
    simd_float3 diffuse_color;
    simd_float3 specular_color;
    
    simd_float3 (*shading_func)(simd_float3 raydir,
                                simd_float3 N,
                                simd_float3 P,
                                Light* l);
    
//    simd_float3 (*shading_func)(Ray ray,
//                                Hit x,
//                                Light l,
//                                struct Material m);
    
    float roughness;
    float fresnel;
    float density;
    float reflectivity;
    float refractivity;
    float ior;
};

typedef struct Material Material;

Material newMaterial(simd_float3 ambient_color,
                     simd_float3 diffuse_color,
                     simd_float3 specular_color,
                     float roughness,
                     float fresnel,
                     float density,
                     float reflectivity,
                     float refractivity,
                     float ior,
//                     simd_float3 (*shading_func)(Ray ray,
//                                                 Hit x,
//                                                 Light l,
//                                                 struct Material m)
                     simd_float3 (*shading_func)(simd_float3 raydir,
                                                 simd_float3 N,
                                                 simd_float3 P,
                                                 Light* l)
                     ) {
    Material material;
    
    material.ambient_color = ambient_color;
    material.diffuse_color = diffuse_color;
    material.specular_color = specular_color;
    material.roughness = roughness;
    material.fresnel = fresnel;
    material.density = density;
    material.reflectivity = reflectivity;
    material.refractivity = refractivity;
    material.ior = ior;
    material.shading_func = shading_func;
    
    return material;
}

simd_float3 shade(Ray ray,
                  Hit x,
                  Light l,
                  Material m) {
    simd_float3 C, N = x.normal, L, V, H;
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
    //    subVec(&l->position, P, &L);
    L = l.position - x.position;
        //    L_length = mag(&L);
    L_length = simd_length(L);
    //    normalizeSelf(&L);
    L = simd_normalize(L);
        //    mulfv(-1.0f, raydir, &V);
    V = -1 * ray.direction;
        //    addVec(&V, &L, &H);
        //    normalize(&H, &H);
    H = V + L;
    H = simd_normalize(H);
    
    NdotL = fminf(simd_dot(N, L), 1.0f);
    NdotV = fminf(simd_dot(N, V), 1.0f);
    NdotH = fminf(simd_dot(N, H), 1.0f);
    VdotH = fminf(simd_dot(V, H), 1.0f);
    
    if (NdotL < epsilon) {
        return C;
    }
    
    // Get the surface properties
    float R = m.roughness;
    float F = m.fresnel;
    float K = m.density;

    // Evaluate the geometric term
    float geo_numerator = 2.0f * NdotH;
    float geo_a = (geo_numerator * NdotV) / VdotH;
    float geo_b = (geo_numerator * NdotL) / VdotH;
    lambertian = fminf(1.0f, fminf(geo_a, geo_b));

    // Evaluate the roughness term
    float roughness_a = 1.0f / (4.0f * R * R * powf(NdotH, 4));
    float roughness_b = NdotH * NdotH - 1.0f;
    float roughness_c = R * R * NdotH * NdotH;
    roughness = roughness_a * expf(roughness_b / roughness_c);

    // Evaluate the fresnel term
    specular = powf(1.0f - VdotH, 5);
    specular *= 1.0f - F;
    specular += F;

    // Put all the terms together
    float Rs_numerator = lambertian * roughness * specular;
    float Rs_denominator = fmaxf(NdotV * NdotL, epsilon);
    float Rs = Rs_numerator / Rs_denominator;
    
    simd_float3 final_a, final_b;
    final_a = l.color * NdotL;
    final_b = (m.diffuse_color * K) + (m.specular_color * (Rs * (1.0f - K)));
    C = final_a * final_b;
    return C / A;
}

simd_float3 shade_Diffuse_White_Fast(simd_float3 raydir,
                                     simd_float3 N,
                                     simd_float3 P,
                                     Light* l) {
    
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
simd_float3 C = {0, 0, 0}, L, V, H;
//    subVec(&l->position, P, &L);
    L = l->position - P;
//    L_length = mag(&L);
    L_length = simd_length(L);
//    normalizeSelf(&L);
    L = simd_normalize(L);
//    mulfv(-1.0f, raydir, &V);
    V = -1 * raydir;
//    addVec(&V, &L, &H);
//    normalize(&H, &H);
    H = V + L;
    H = simd_normalize(H);
    
    A = l->constant + l->linear * L_length + l->exponent * L_length * L_length + epsilon;
    
    //Calculate the dot products required for shading
//    NdotL = fminf(dot(N, &L), 1.0f);
    NdotL = fminf(simd_dot(N, L), 1.0f);
//    NdotV = fminf(dot(N, &V), 1.0f);
    NdotV = fminf(simd_dot(N, V), 1.0f);
    NdotH = fminf(simd_dot(N, H), 1.0f);
    VdotH = fminf(simd_dot(V, H), 1.0f);
    
    if (NdotL < epsilon) {
        return C;
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
    float tfloat = Rs * 0.1f + 0.9f;
    C = l->color * NdotL * tfloat / A;
//    printVec(C);

    return C;
}

Material Diffuse_White() {
    simd_float3 ac = {0.01f, 0.01f, 0.01f};
    simd_float3 dc = {1, 1, 1};
    simd_float3 sc = {1, 1, 1};
    float roughness = 0.375;
    float fresnel = 0.5f;
    float density = 0.9f;
    float reflectivity = 0.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, roughness, fresnel, density, reflectivity, refractivity, ior,
                       shade_Diffuse_White_Fast
//                       shade
                       );
}

simd_float3 shade_Diffuse_Red_Fast(simd_float3 raydir,
                                   simd_float3 N,
                                   simd_float3 P,
                                   Light* l) {
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
simd_float3 C = {0, 0, 0}, L, V, H;
    //    subVec(&l->position, P, &L);
        L = l->position - P;
    //    L_length = mag(&L);
        L_length = simd_length(L);
    //    normalizeSelf(&L);
        L = simd_normalize(L);
    //    mulfv(-1.0f, raydir, &V);
        V = -1 * raydir;
    //    addVec(&V, &L, &H);
    //    normalize(&H, &H);
        H = V + L;
        H = simd_normalize(H);
        
        A = l->constant + l->linear * L_length + l->exponent * L_length * L_length + epsilon;
        
        //Calculate the dot products required for shading
    //    NdotL = fminf(dot(N, &L), 1.0f);
        NdotL = fminf(simd_dot(N, L), 1.0f);
    //    NdotV = fminf(dot(N, &V), 1.0f);
        NdotV = fminf(simd_dot(N, V), 1.0f);
        NdotH = fminf(simd_dot(N, H), 1.0f);
        VdotH = fminf(simd_dot(V, H), 1.0f);
        
        if (NdotL < epsilon) {
            return C;
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
//    mulfv(NdotL / A, &l->color, a);
    C = l->color * NdotL / A;

    float tfloat = Rs * (1.0f - K);

    simd_float3 final_b = {tfloat+K, tfloat, tfloat};

//    mulvv(&final_b, a, a);
    C = C * final_b;
    return C;
}

Material Diffuse_Red() {
    simd_float3 ac = {0.01f, 0.01f, 0.01f};
    simd_float3 dc = {1, 0, 0};
    simd_float3 sc = {1, 1, 1};
    float roughness = 0.375;
    float fresnel = 0.5f;
    float density = 0.9f;
    float reflectivity = 0.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, roughness, fresnel, density, reflectivity, refractivity, ior,
                       shade_Diffuse_Red_Fast
//                       shade
                       );
}

simd_float3 shade_Diffuse_Blue_Fast(simd_float3 raydir,
                                    simd_float3 N,
                                    simd_float3 P,
                                    Light* l)
{
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
simd_float3 C = {0, 0, 0}, L, V, H;
    //    subVec(&l->position, P, &L);
        L = l->position - P;
    //    L_length = mag(&L);
        L_length = simd_length(L);
    //    normalizeSelf(&L);
        L = simd_normalize(L);
    //    mulfv(-1.0f, raydir, &V);
        V = -1 * raydir;
    //    addVec(&V, &L, &H);
    //    normalize(&H, &H);
        H = V + L;
        H = simd_normalize(H);
        
        A = l->constant + l->linear * L_length + l->exponent * L_length * L_length + epsilon;
        
        //Calculate the dot products required for shading
    //    NdotL = fminf(dot(N, &L), 1.0f);
        NdotL = fminf(simd_dot(N, L), 1.0f);
    //    NdotV = fminf(dot(N, &V), 1.0f);
        NdotV = fminf(simd_dot(N, V), 1.0f);
        NdotH = fminf(simd_dot(N, H), 1.0f);
        VdotH = fminf(simd_dot(V, H), 1.0f);
        
        if (NdotL < epsilon) {
            return C;
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
    //    mulfv(NdotL / A, &l->color, a);
    C = l->color * NdotL / A;

    float tfloat = Rs * (1.0f - K);

    simd_float3 final_b = {tfloat, tfloat, tfloat+K};

//    mulvv(&final_b, a, a);
    C = C * final_b;
    return C;
}

Material Diffuse_Blue() {
    simd_float3 ac = {0.01f, 0.01f, 0.01f};
    simd_float3 dc = {0, 0, 1};
    simd_float3 sc = {1, 1, 1};
    float roughness = 0.375;
    float fresnel = 0.5f;
    float density = 0.9f;
    float reflectivity = 0.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, roughness, fresnel, density, reflectivity, refractivity, ior,
                       shade_Diffuse_Blue_Fast
//                       shade
                       );
}

simd_float3 shade_Reflective_Metal_Fast(simd_float3 raydir,
                                        simd_float3 N,
                                        simd_float3 P,
                                        Light* l)
{
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
simd_float3 C = {0, 0, 0}, L, V, H;
    //    subVec(&l->position, P, &L);
        L = l->position - P;
    //    L_length = mag(&L);
        L_length = simd_length(L);
    //    normalizeSelf(&L);
        L = simd_normalize(L);
    //    mulfv(-1.0f, raydir, &V);
        V = -1 * raydir;
    //    addVec(&V, &L, &H);
    //    normalize(&H, &H);
        H = V + L;
        H = simd_normalize(H);
        
        A = l->constant + l->linear * L_length + l->exponent * L_length * L_length + epsilon;
        
        //Calculate the dot products required for shading
    //    NdotL = fminf(dot(N, &L), 1.0f);
        NdotL = fminf(simd_dot(N, L), 1.0f);
    //    NdotV = fminf(dot(N, &V), 1.0f);
        NdotV = fminf(simd_dot(N, V), 1.0f);
        NdotH = fminf(simd_dot(N, H), 1.0f);
        VdotH = fminf(simd_dot(V, H), 1.0f);
        
        if (NdotL < epsilon) {
            return C;
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

//    mulfv(NdotL * tfloat / A, &l->color, a);
    C = l->color * NdotL * tfloat / A;
    return C;
}

Material Reflective_Metal() {
    simd_float3 ac = {0.01f, 0.01f, 0.01f};
    simd_float3 dc = {0, 0, 0};
    simd_float3 sc = {1, 1, 1};
    float roughness = 0.1f;
    float fresnel = 1.0f;
    float density = 0.5f;
    float reflectivity = 1.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, roughness, fresnel, density, reflectivity, refractivity, ior,
                       shade_Reflective_Metal_Fast
//                       shade
                       );
}

simd_float3 shade_Refractive_Glass_Fast(simd_float3 raydir,
                                        simd_float3 N,
                                        simd_float3 P,
                                        Light* l)
{
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
    simd_float3 C = {0, 0, 0}, L, V, H;
        
    //    subVec(&l->position, P, &L);
        L = l->position - P;
    //    L_length = mag(&L);
        L_length = simd_length(L);
    //    normalizeSelf(&L);
        L = simd_normalize(L);
    //    mulfv(-1.0f, raydir, &V);
        V = -1 * raydir;
    //    addVec(&V, &L, &H);
    //    normalize(&H, &H);
        H = V + L;
        H = simd_normalize(H);
        
        A = l->constant + l->linear * L_length + l->exponent * L_length * L_length + epsilon;
        
        //Calculate the dot products required for shading
    //    NdotL = fminf(dot(N, &L), 1.0f);
        NdotL = fminf(simd_dot(N, L), 1.0f);
    //    NdotV = fminf(dot(N, &V), 1.0f);
        NdotV = fminf(simd_dot(N, V), 1.0f);
        NdotH = fminf(simd_dot(N, H), 1.0f);
        VdotH = fminf(simd_dot(V, H), 1.0f);
        
        if (NdotL < epsilon) {
            return C;
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
    
    //    mulfv(NdotL * tfloat / A, &l->color, a);
    C = l->color * NdotL * tfloat / A;
    return C;}

Material Refractive_Glass() {
    simd_float3 ac = {0.01f, 0.01f, 0.01f};
    simd_float3 dc = {0, 0, 0};
    simd_float3 sc = {1, 1, 1};
    float roughness = 0.1f;
    float fresnel = 1.0f;
    float density = 0.5f;
    float reflectivity = 0.0f;
    float refractivity = 1.0f;
    float ior = 2.0f;
    return newMaterial(ac, dc, sc, roughness, fresnel, density, reflectivity, refractivity, ior,
                       shade_Refractive_Glass_Fast
//                       shade
                       );
}

simd_float3 shade_Diffuse_Sky_Blue_Fast(simd_float3 raydir,
                                        simd_float3 N,
                                        simd_float3 P,
                                        Light* l)
{
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
    simd_float3 C = {0, 0, 0}, L, V, H;
    
    //    subVec(&l->position, P, &L);
    L = l->position - P;
    //    L_length = mag(&L);
    L_length = simd_length(L);
//    normalizeSelf(&L);
    L = simd_normalize(L);
    //    mulfv(-1.0f, raydir, &V);
    V = -1 * raydir;
    //    addVec(&V, &L, &H);
    //    normalize(&H, &H);
    H = V + L;
    H = simd_normalize(H);
    
    A = l->constant + l->linear * L_length + l->exponent * L_length * L_length + epsilon;
        
        //Calculate the dot products required for shading
    //    NdotL = fminf(dot(N, &L), 1.0f);
    NdotL = fminf(simd_dot(N, L), 1.0f);
    //    NdotV = fminf(dot(N, &V), 1.0f);
    NdotV = fminf(simd_dot(N, V), 1.0f);
    NdotH = fminf(simd_dot(N, H), 1.0f);
    VdotH = fminf(simd_dot(V, H), 1.0f);
        
    if (NdotL < epsilon) {
        return C;
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
    simd_float3 final_a;
    
//    mulfv(NdotL, &l->color, &final_a);
    final_a = NdotL * l->color;

    simd_float3 final_b;
        
    simd_float3 dc = {0.2f, 0.2f, 0.9f};
//    mulfv(K, &dc, &temp2);
        
//    simd_float3 temp3;
    
    float tfloat = Rs * (1.0f - K);
    
    simd_float3 sc = {1, 1, 1};
    
//    mulfv(tfloat, &sc, &temp3);
//    temp3 = tfloat * sc;
        
//    addVec(&temp2, &temp3, &final_b);
//    final_b = dc + temp3;
    
    final_b = dc * K + sc *Rs * (1.0f - K);
    
//    mulvv(&final_a, &final_b, &C);
    C = final_a * final_b;
    C /= A;
//    divideFloat(&C, A, a);
    return C;
}

Material Diffuse_Sky_Blue() {
    simd_float3 ac = {0.2f, 0.25f, 0.75f};
    simd_float3 dc = {0.2f, 0.2f, 0.9f};
    simd_float3 sc = {1, 1, 1};
    float roughness = 0.9f;
    float fresnel = 0.5f;
    float density = 0.9f;
    float reflectivity = 0.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, roughness, fresnel, density, reflectivity, refractivity, ior,
                       shade_Diffuse_Sky_Blue_Fast
//                       shade
                       );
}


#endif /* Materials_h */

