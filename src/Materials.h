//
//  Materials.h
//  SDL_ACCEL6
//
//  Created by Ben on 8/6/20.
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

typedef struct Material {
    vec ambient_color;
    vec diffuse_color;
    vec specular_color;
    float roughness;
    float fresnel;
    float density;
    float reflectivity;
    float refractivity;
    float ior;
//    void (*shading_func)(float * restrict V, float * restrict N, float * restrict P, Light l, float * restrict a);
    void (*shading_func)(Ray r, Hit x, Light l
//                         , struct Material m
                         , float * restrict a);
} Material;

Material newMaterial(vec ambient_color, vec diffuse_color, vec specular_color, float roughness, float fresnel, float density, float reflectivity, float refractivity, float ior,
//                     void (*shading_func)(float * restrict V, float * restrict N, float * restrict P, Light l, float * restrict a)
                     void (*shading_func)(Ray r, Hit x, Light l
//                                          , struct Material m
                                          , float * restrict a)
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

void shadeP1(Ray r, Hit x, Light l, float* NdotL, float* NdotV, float* NdotH, float* VdotH, float* A) {
    vec N, L, V, H;
    N = x.normal;
    float L_length;
    //    L = l.getPos().sub(x.getPos());
    subVec(&l.position, &x.position, &L);
    //    L_length = L.length();
    L_length = mag(&L);
    //    L = L.normalize();
    normalizeSelf(&L);
    //    V = r.getDir().negate();
    mulfv(-1.0f, &r.direction, &V);
    //    H = V.add(L).normalize();
    vec temp1;
    addVec(&V, &L, &temp1);
    normalize(&temp1, &H);
        
    A[0] = l.constant + (l.linear * L_length) + l.exponent * L_length * L_length + epsilon;
        
    //Calculate the dot products required for shading
    *NdotL = fminf(dot(&N, &L), 1.0f);
    *NdotV = fminf(dot(&N, &V), 1.0f);
    *NdotH = fminf(dot(&N, &H), 1.0f);
    *VdotH = fminf(dot(&V, &H), 1.0f);
    
}

void shade(Ray r, Hit x, Light l, Material m,  float * restrict a) {
////    vec N, L, V, H;
//    N = x.normal;
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
////    L = l.getPos().sub(x.getPos());
//    subVec(&l.position, &x.position, &L);
////    L_length = L.length();
//    L_length = mag(&L);
////    L = L.normalize();
//    normalizeSelf(&L);
////    V = r.getDir().negate();
//    mulfv(-1.0f, &r.direction, &V);
////    H = V.add(L).normalize();
//    vec temp1;
//    addVec(&V, &L, &temp1);
//    normalize(&temp1, &H);
//
//    A = l.constant + l.linear * L_length + l.exponent * L_length * L_length + epsilon;
//
//    //Calculate the dot products required for shading
//    NdotL = fminf(dot(&N, &L), 1.0f);
//    NdotV = fminf(dot(&N, &V), 1.0f);
//    NdotH = fminf(dot(&N, &H), 1.0f);
//    VdotH = fminf(dot(&V, &H), 1.0f);
    shadeP1(r, x, l, &NdotL, &NdotV, &NdotH, &VdotH, &A);
    vec C = {0, 0, 0};
    
    if (NdotL < epsilon) {
        mulfvSelf(0, a);
        return;
//        return C;
    }
    
    float R = m.roughness;
    float F = m.fresnel;
    float K = m.density;
    
    // Evaluate the geometric term
    float geo_numerator = 2.0f * NdotH;
    float geo_a = (geo_numerator * NdotV) / VdotH;
    float geo_b = (geo_numerator * NdotL) / VdotH;
//    lambertian = Math.min(1.0f, Math.min(geo_a, geo_b));
    float f1 = fminf(geo_a, geo_b);
    lambertian = fminf(1.0f, f1);
    
    // Evaluate the roughness term
    float roughness_a = 1.0f / (4.0f * R * R * (float) powf(NdotH, 4));
    float roughness_b = NdotH * NdotH - 1.0f;
    float roughness_c = R * R * NdotH * NdotH;
//    roughness = roughness_a * (float) Math.exp(roughness_b / roughness_c);
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
//    Vec3f final_a = l.getColor().scale(NdotL).scale(l.getIntensity());
    vec final_a;
    mulfv(NdotL, &l.color, &final_a);

//    Vec3f final_b = m.getColorDiffuse().scale(K).add(m.getColorSpecular().scale(Rs * (1.0f - K)));
    vec final_b;
    
    vec temp2;
    mulfv(K, &m.diffuse_color, &temp2);
    
    vec temp3;
    float tfloat = Rs * (1.0f - K);
    mulfv(tfloat, &m.specular_color, &temp3);
    
    addVec(&temp2, &temp3, &final_b);
    
    
//    C.set(final_a.scale(final_b));
    mulvv(&final_a, &final_b, &C);
    
//    return C.divide(A);
    
//    divideFloat(&C, A, &C);
    divideFloat(&C, A, a);
}

void shade_Diffuse_White(Ray r, Hit x, Light l,
//                         Material m,
                         float * restrict a) {
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
    shadeP1(r, x, l, &NdotL, &NdotV, &NdotH, &VdotH, &A);
    vec C = {0, 0, 0};
    
    if (NdotL < epsilon) {
        mulfvSelf(0, a);
        return;
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
//    lambertian = Math.min(1.0f, Math.min(geo_a, geo_b));
    float f1 = fminf(geo_a, geo_b);
    lambertian = fminf(1.0f, f1);
        
    // Evaluate the roughness term
    float roughness_a = 1.0f / (4.0f * R * R * (float) powf(NdotH, 4));
    float roughness_b = NdotH * NdotH - 1.0f;
    float roughness_c = R * R * NdotH * NdotH;
//    roughness = roughness_a * (float) Math.exp(roughness_b / roughness_c);
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
    float tfloat = Rs * (1.0f - K) + K;
    mulfv(NdotL * tfloat / A, &l.color, a);
}

Material Diffuse_White() {
//    vec a = {0.01f, 0.01f, 0.01f, 0};
//    vec b = {1, 1, 1, 0};
//    return newMaterial(a, b, b, 0.375f, 0.5f, 0.9f, 0.0f, 0.0f, 0.0f, shade_Diffuse_White);
    vec ac = {0.01f, 0.01f, 0.01f};
    vec dc = {1, 1, 1};
    vec sc = {1, 1, 1};
    float roughness = 0.375;
    float fresnel = 0.5f;
    float density = 0.9f;
    float reflectivity = 0.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, roughness, fresnel, density, reflectivity, refractivity, ior, shade_Diffuse_White);
}

void shade_Diffuse_Red(Ray r, Hit x, Light l,
//                         Material m,
                         float * restrict a) {
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
    shadeP1(r, x, l, &NdotL, &NdotV, &NdotH, &VdotH, &A);
    vec C = {0, 0, 0};
        
    if (NdotL < epsilon) {
        mulfvSelf(0, a);
        return;
    //        return C;
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
//    lambertian = Math.min(1.0f, Math.min(geo_a, geo_b));
    float f1 = fminf(geo_a, geo_b);
    lambertian = fminf(1.0f, f1);
        
    // Evaluate the roughness term
    float roughness_a = 1.0f / (4.0f * R * R * (float) powf(NdotH, 4));
    float roughness_b = NdotH * NdotH - 1.0f;
    float roughness_c = R * R * NdotH * NdotH;
    //    roughness = roughness_a * (float) Math.exp(roughness_b / roughness_c);
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
    mulfv(NdotL / A, &l.color, a);

    float tfloat = Rs * (1.0f - K);

    vec final_b = {tfloat+K, tfloat, tfloat};

    mulvv(&final_b, a, a);
}

Material Diffuse_Red() {
//    vec a = {0.01f, 0.01f, 0.01f, 0};
//    vec b = {1, 0, 0, 0};
//    vec c = {1, 1, 1, 0};
//    return newMaterial(a, b, c, 0.375, 0.5, 0.9, 0.0, 0.0, 0.0, shade_Diffuse_Red);
    vec ac = {0.01f, 0.01f, 0.01f};
    vec dc = {1, 0, 0};
    vec sc = {1, 1, 1};
    float roughness = 0.375;
    float fresnel = 0.5f;
    float density = 0.9f;
    float reflectivity = 0.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, roughness, fresnel, density, reflectivity, refractivity, ior, shade_Diffuse_Red);
}

void shade_Diffuse_Blue(Ray r, Hit x, Light l,
//                         Material m,
                         float * restrict a) {
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
    shadeP1(r, x, l, &NdotL, &NdotV, &NdotH, &VdotH, &A);
    vec C = {0, 0, 0};
        
    if (NdotL < epsilon) {
        mulfvSelf(0, a);
        return;
    //        return C;
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
//    lambertian = Math.min(1.0f, Math.min(geo_a, geo_b));
    float f1 = fminf(geo_a, geo_b);
    lambertian = fminf(1.0f, f1);
        
    // Evaluate the roughness term
    float roughness_a = 1.0f / (4.0f * R * R * (float) powf(NdotH, 4));
    float roughness_b = NdotH * NdotH - 1.0f;
    float roughness_c = R * R * NdotH * NdotH;
    //    roughness = roughness_a * (float) Math.exp(roughness_b / roughness_c);
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
    mulfv(NdotL / A, &l.color, a);

    float tfloat = Rs * (1.0f - K);

    vec final_b = {tfloat, tfloat, tfloat+K};

    mulvv(&final_b, a, a);
}

Material Diffuse_Blue() {
//    vec a = {0.01f, 0.01f, 0.01f, 0};
//    vec b = {0, 0, 1, 0};
//    vec c = {1, 1, 1, 0};
//    return newMaterial(a, b, c, 0.375, 0.5, 0.9, 0.0, 0.0, 0.0, shade_Diffuse_Blue);
    vec ac = {0.01f, 0.01f, 0.01f};
    vec dc = {0, 0, 1};
    vec sc = {1, 1, 1};
    float roughness = 0.375;
    float fresnel = 0.5f;
    float density = 0.9f;
    float reflectivity = 0.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, roughness, fresnel, density, reflectivity, refractivity, ior, shade_Diffuse_Blue);
}

void shade_Reflective_Metal(Ray r, Hit x, Light l,
//                         Material m,
                         float * restrict a) {
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
    shadeP1(r, x, l, &NdotL, &NdotV, &NdotH, &VdotH, &A);
    vec C = {0, 0, 0};
    
    if (NdotL < epsilon) {
        mulfvSelf(0, a);
        return;
    //        return C;
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
    //    lambertian = Math.min(1.0f, Math.min(geo_a, geo_b));
    float f1 = fminf(geo_a, geo_b);
    lambertian = fminf(1.0f, f1);
        
    // Evaluate the roughness term
    float roughness_a = 1.0f / (4.0f * R * R * (float) powf(NdotH, 4));
    float roughness_b = NdotH * NdotH - 1.0f;
    float roughness_c = R * R * NdotH * NdotH;
    //    roughness = roughness_a * (float) Math.exp(roughness_b / roughness_c);
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
    //    Vec3f final_a = l.getColor().scale(NdotL).scale(l.getIntensity());
//    vec final_a;
//    mulfv(NdotL, &l.color, &final_a);
    //    Vec3f final_b = m.getColorDiffuse().scale(K).add(m.getColorSpecular().scale(Rs * (1.0f - K)));

        
//    vec temp2;
//    vec dc = {0, 0, 0};
//    mulfv(K, &dc, &temp2);
//    vec temp2 = {0, 0, 0};
    
    float tfloat = Rs * (1.0f - K);
//    mulfv(tfloat, &sc, &final_b);
//    vec final_b = {tfloat, tfloat, tfloat};
//    addVec(&temp2, &temp3, &final_b);
    
        
    //    C.set(final_a.scale(final_b));
//    mulvv(&final_a, &final_b, &C);
        
    //    return C.divide(A);
    mulfv(NdotL * tfloat / A, &l.color, a);
    //    divideFloat(&C, A, &C);
//    divideFloat(&C, A, a);
}

Material Reflective_Metal() {
//    vec a = {0.01f, 0.01f, 0.01f, 0};
//    vec b = {0, 0, 0, 0};
//    vec c = {1, 1, 1, 0};
//    return newMaterial(a, b, c, 0.1, 1.0, 0.5, 1.0, 0.0, 0.0, shade_Reflective_Metal);
    vec ac = {0.01f, 0.01f, 0.01f};
    vec dc = {0, 0, 0};
    vec sc = {1, 1, 1};
    float roughness = 0.1f;
    float fresnel = 1.0f;
    float density = 0.5f;
    float reflectivity = 1.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, roughness, fresnel, density, reflectivity, refractivity, ior, shade_Reflective_Metal);
}

void shade_Refractive_Glass(Ray r, Hit x, Light l,
//                         Material m,
                         float * restrict a) {
    float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
    shadeP1(r, x, l, &NdotL, &NdotV, &NdotH, &VdotH, &A);
    vec C = {0, 0, 0};
    
    if (NdotL < epsilon) {
        mulfvSelf(0, a);
        return;
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
//    lambertian = Math.min(1.0f, Math.min(geo_a, geo_b));
    float f1 = fminf(geo_a, geo_b);
    lambertian = fminf(1.0f, f1);
        
    // Evaluate the roughness term
    float roughness_a = 1.0f / (4.0f * R * R * (float) powf(NdotH, 4));
    float roughness_b = NdotH * NdotH - 1.0f;
    float roughness_c = R * R * NdotH * NdotH;
//    roughness = roughness_a * (float) Math.exp(roughness_b / roughness_c);
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
        //    Vec3f final_a = l.getColor().scale(NdotL).scale(l.getIntensity());
    //    vec final_a;
    //    mulfv(NdotL, &l.color, &final_a);
        //    Vec3f final_b = m.getColorDiffuse().scale(K).add(m.getColorSpecular().scale(Rs * (1.0f - K)));

            
    //    vec temp2;
    //    vec dc = {0, 0, 0};
    //    mulfv(K, &dc, &temp2);
    //    vec temp2 = {0, 0, 0};
        
        float tfloat = Rs * (1.0f - K);
    //    mulfv(tfloat, &sc, &final_b);
    //    vec final_b = {tfloat, tfloat, tfloat};
    //    addVec(&temp2, &temp3, &final_b);
        
            
        //    C.set(final_a.scale(final_b));
    //    mulvv(&final_a, &final_b, &C);
            
        //    return C.divide(A);
        mulfv(NdotL * tfloat / A, &l.color, a);
        //    divideFloat(&C, A, &C);
    //    divideFloat(&C, A, a);
}

Material Refractive_Glass() {
//    vec a = {0.01f, 0.01f, 0.01f, 0};
//    vec b = {0, 0, 0, 0};
//    vec c = {1, 1, 1, 0};
//    return newMaterial(a, b, c, 0.1f, 1.0f, 0.5f, 0.0f, 1.0f, 2.0f, shade_Refractive_Glass);
    vec ac = {0.01f, 0.01f, 0.01f};
    vec dc = {0, 0, 0};
    vec sc = {1, 1, 1};
    float roughness = 0.1f;
    float fresnel = 1.0f;
    float density = 0.5f;
    float reflectivity = 0.0f;
    float refractivity = 1.0f;
    float ior = 2.0f;
    return newMaterial(ac, dc, sc, roughness, fresnel, density, reflectivity, refractivity, ior, shade_Refractive_Glass);
}

void shade_Diffuse_Sky_Blue(Ray r, Hit x, Light l,
//                         Material m,
                         float * restrict a) {
        float NdotL, NdotV, NdotH, VdotH, lambertian, specular, roughness, L_length, A = 1.0f;
    shadeP1(r, x, l, &NdotL, &NdotV, &NdotH, &VdotH, &A);
    vec C = {0, 0, 0};
        
    if (NdotL < epsilon) {
        mulfvSelf(0, a);
        return;
    //        return C;
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
//    lambertian = Math.min(1.0f, Math.min(geo_a, geo_b));
    float f1 = fminf(geo_a, geo_b);
    lambertian = fminf(1.0f, f1);
        
    // Evaluate the roughness term
    float roughness_a = 1.0f / (4.0f * R * R * (float) powf(NdotH, 4));
    float roughness_b = NdotH * NdotH - 1.0f;
    float roughness_c = R * R * NdotH * NdotH;
    //    roughness = roughness_a * (float) Math.exp(roughness_b / roughness_c);
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
    //    Vec3f final_a = l.getColor().scale(NdotL).scale(l.getIntensity());
    vec final_a;
    mulfv(NdotL, &l.color, &final_a);

    //    Vec3f final_b = m.getColorDiffuse().scale(K).add(m.getColorSpecular().scale(Rs * (1.0f - K)));
    vec final_b;
        
    vec temp2;
    vec dc = {0.2f, 0.2f, 0.9f};
    mulfv(K, &dc, &temp2);
        
    vec temp3;
    float tfloat = Rs * (1.0f - K);
    vec sc = {1, 1, 1};
    mulfv(tfloat, &sc, &temp3);
        
    addVec(&temp2, &temp3, &final_b);
    
        
    //    C.set(final_a.scale(final_b));
    mulvv(&final_a, &final_b, &C);
    
    //    return C.divide(A);
        
    //    divideFloat(&C, A, &C);
    divideFloat(&C, A, a);}

Material Diffuse_Sky_Blue() {
//    vec a = {0.2f, 0.25f, 0.75f, 0};
//    vec b = {0.2f, 0.2f, 0.9f, 0};
//    vec c = {1, 1, 1, 0};
//    return newMaterial(a, b, c, 0.375, 0.5, 0.9, 0.0, 0.0, 0.0, shade_Diffuse_Sky_Blue);
    vec ac = {0.2f, 0.25f, 0.75f};
    vec dc = {0.2f, 0.2f, 0.9f};
    vec sc = {1, 1, 1};
    float roughness = 0.9f;
    float fresnel = 0.5f;
    float density = 0.9f;
    float reflectivity = 0.0f;
    float refractivity = 0.0f;
    float ior = 0.0f;
    return newMaterial(ac, dc, sc, roughness, fresnel, density, reflectivity, refractivity, ior, shade_Diffuse_Sky_Blue);
}

#endif /* Materials_h */

