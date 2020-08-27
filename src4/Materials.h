//
//  Materials.h
//  SDL_ACCEL13
//
//  Created by Ben on 8/26/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Materials_h
#define Materials_h

#include <stdio.h>
#include "Light.h"
#include "Settings.h"

#define calc_lamb(NdotV, NdotL, NdotH, VdotH) (fminf(1.0f, fminf(NdotV, NdotL) * 2.0f * NdotH / VdotH))

#define calc_rough(NdotH, r1) expf((NdotH * NdotH - 1.0f)/r1/r1/NdotH/NdotH) / (r1 * r1 * 4 * powf(NdotH, 4))

#define calc_spec(VdotH, F) (powf(1.0f - VdotH, 5) * (1.0f - F) + F)

#define calc_Rs(NdotV, NdotL, NdotH, VdotH, r1, F) ( \
calc_lamb(NdotV, NdotL, NdotH, VdotH) * \
calc_rough(NdotH, r1) * \
calc_spec(VdotH, F) / \
fmaxf(NdotV * NdotL, epsilon) \
)

struct __attribute__ ((__packed__)) Material {
    simd_float3 ambient_color;
    simd_float3 (*shading_func)(const simd_float3 raydir,
                                const simd_float3 N,
                                const simd_float3 P,
                                const Light* l);
    float reflectivity;
    float refractivity;
    float ior;
};

typedef struct Material Material;

Material newMaterial(simd_float3 ambient_color,
                     simd_float3 diffuse_color,
                     simd_float3 specular_color,
                     float reflectivity,
                     float refractivity,
                     float ior,
                     simd_float3 (*shading_func)(const simd_float3 raydir,
                                                 const simd_float3 N,
                                                 const simd_float3 P,
                                                 const Light* l)
                     ) {
    Material material;
    
    material.ambient_color = ambient_color;
    material.reflectivity = reflectivity;
    material.refractivity = refractivity;
    material.ior = ior;
    material.shading_func = shading_func;
    
    return material;
}

#define shade_header float NdotL, NdotV, NdotH, VdotH, L_length, A = 1.0f; \
simd_float3 C = {0, 0, 0}, L, V, H; \
L = l->position - P; \
L_length = simd_length(L); \
L = simd_normalize(L); \
V = -1 * raydir; \
H = simd_normalize(V + L); \
A = l->constant + l->linear * L_length + l->exponent * L_length * L_length + epsilon;

//Calculate the dot products required for shading
#define dot_products \
NdotL = fminf(simd_dot(N, L), 1.0f); \
NdotV = fminf(simd_dot(N, V), 1.0f); \
NdotH = fminf(simd_dot(N, H), 1.0f); \
VdotH = fminf(simd_dot(V, H), 1.0f);

#define cutoff \
if (NdotL < epsilon) { \
    return C; \
}

#define last_part(diffuse_color) \
float tfloat = calc_Rs(NdotV, NdotL, NdotH, VdotH, R, F) * (1.0f - K); \
simd_float3 final_b = diffuse_color + tfloat; \
return l->color * NdotL / A * final_b;


#define Diffuse_White_RFK \
float R = 0.375; \
float F = 0.5; \
float K = 0.9f; \
float diffuse_color = K;


#define shade_color \
shade_header; \
dot_products; \
cutoff; \
last_part(diffuse_color);

simd_float3 shade_Diffuse_White_Fast(const simd_float3 raydir,
                                     const simd_float3 N,
                                     const simd_float3 P,
                                     const Light* l) {
    Diffuse_White_RFK;
    shade_color;
}

Material Diffuse_White() {
    simd_float3 ac = {0.01f, 0.01f, 0.01f};
    simd_float3 dc = {1, 1, 1};
    simd_float3 sc = {1, 1, 1};
    float reflectivity = 0.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, reflectivity, refractivity, ior, shade_Diffuse_White_Fast);
}

#define Diffuse_Red_RFK \
float R = 0.375; \
float F = 0.5; \
float K = 0.9f; \
simd_float3 diffuse_color = {K, 0, 0};

simd_float3 shade_Diffuse_Red_Fast(const simd_float3 raydir,
                                   const simd_float3 N,
                                   const simd_float3 P,
                                   const Light* l) {
    Diffuse_Red_RFK;
    shade_color;
}

Material Diffuse_Red() {
    simd_float3 ac = {0.01f, 0.01f, 0.01f};
    simd_float3 dc = {1, 0, 0};
    simd_float3 sc = {1, 1, 1};
    float reflectivity = 0.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, reflectivity, refractivity, ior, shade_Diffuse_Red_Fast);
}

#define Diffuse_Blue_RFK \
float R = 0.375; \
float F = 0.5; \
float K = 0.9f; \
simd_float3 diffuse_color = {0, 0, K};

simd_float3 shade_Diffuse_Blue_Fast(const simd_float3 raydir,
                                    const simd_float3 N,
                                    const simd_float3 P,
                                    const Light* l) {
    Diffuse_Blue_RFK;
    shade_color;
}

Material Diffuse_Blue() {
    simd_float3 ac = {0.01f, 0.01f, 0.01f};
    simd_float3 dc = {0, 0, 1};
    simd_float3 sc = {1, 1, 1};
    float reflectivity = 0.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, reflectivity, refractivity, ior, shade_Diffuse_Blue_Fast);
}

#define Reflective_Metal_RFK \
float R = 0.1F; \
float F = 1.0f; \
float K = 0.5f; \
float diffuse_color = 0;

simd_float3 shade_Reflective_Metal_Fast(const simd_float3 raydir,
                                        const simd_float3 N,
                                        const simd_float3 P,
                                        const Light* l) {
    Reflective_Metal_RFK;
    shade_color;
}

Material Reflective_Metal() {
    simd_float3 ac = {0.01f, 0.01f, 0.01f};
    simd_float3 dc = {0, 0, 0};
    simd_float3 sc = {1, 1, 1};
    float reflectivity = 1.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, reflectivity, refractivity, ior, shade_Reflective_Metal_Fast);
}

#define Refractive_Glass_RFK \
float R = 0.1F; \
float F = 1.0f; \
float K = 0.5f; \
float diffuse_color = 0;

simd_float3 shade_Refractive_Glass_Fast(const simd_float3 raydir,
                                        const simd_float3 N,
                                        const simd_float3 P,
                                        const Light* l) {
    Refractive_Glass_RFK;
    shade_color
}

Material Refractive_Glass() {
    simd_float3 ac = {0.01f, 0.01f, 0.01f};
    simd_float3 dc = {0, 0, 0};
    simd_float3 sc = {1, 1, 1};
    float reflectivity = 0.0f;
    float refractivity = 1.0f;
    float ior = 2.0f;
    return newMaterial(ac, dc, sc, reflectivity, refractivity, ior, shade_Refractive_Glass_Fast);
}

#define Diffuse_Sky_Blue_RFK \
float R = 0.375; \
float F = 0.5; \
float K = 0.9f; \
simd_float3 diffuse_color = {0.2f, 0.2f, 0.9f};

simd_float3 shade_Diffuse_Sky_Blue_Fast(const simd_float3 raydir,
                                        const simd_float3 N,
                                        const simd_float3 P,
                                        const Light* l) {
    Diffuse_Sky_Blue_RFK;
    shade_color;
}

Material Diffuse_Sky_Blue() {
    simd_float3 ac = {0.2f, 0.25f, 0.75f};
    simd_float3 dc = {0.2f, 0.2f, 0.9f};
    simd_float3 sc = {1, 1, 1};
    float reflectivity = 0.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, reflectivity, refractivity, ior, shade_Diffuse_Sky_Blue_Fast);
}

#endif /* Materials_h */

