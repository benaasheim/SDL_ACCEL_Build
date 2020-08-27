//
//  Camera.h
//  SDL_ACCEL10
//
//  Created by Ben on 8/13/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Camera_h
#define Camera_h

#include <stdio.h>
#include "Ray.h"
#include "Settings.h"

#include <SDL2/SDL.h>

#define AA(i) (((i%window_width) - window_width * 0.5)/window_width*aspect_ratio)
#define BB(i) ((window_height * 0.5 - (i/window_width))/window_height)

struct __attribute__ ((__packed__)) Camera {
    simd_float3 position;
    simd_quatf rot;
    float speed;
    float sensitivity;
};

typedef struct Camera Camera;

Camera newCamera(simd_float3 position, float sp, float se) {
    Camera camera;
    camera.position = position;
    camera.rot.vector.y = 1;
    camera.speed = sp;
    camera.sensitivity = se;
    return camera;
}

void move_camera(Camera* c, simd_float3 dir, float amount) {
    c->position += dir * amount;
}

void rotate_camera(Camera* c, simd_float3 axis, float theta) {
    c->rot = simd_normalize(simd_mul(simd_quaternion(theta, axis), c->rot));
}


simd_float3 camera_Forward(Camera* c) {
    return simd_act(c->rot, (simd_float3){0, 0, 1});
}

simd_float3 camera_Right(Camera* c) {
    return simd_act(c->rot, (simd_float3){1, 0, 0});
}

simd_float3 camera_Up(Camera* c) {
    return simd_act(c->rot, (simd_float3){0, 1, 0});
}

Ray calcCameraRay(Camera* c, int w, int h, float ar, int x, int y) {
    float x_norm = (x - w * 0.5f) / w * ar;
    float y_norm = (h * 0.5f - y) / h;
    
    simd_float3 forward = camera_Forward(c);
    simd_float3 up = camera_Up(c);
    simd_float3 right = camera_Right(c);
    
    Ray ray;
    simd_float3 image_point = (right * x_norm) + (up * y_norm) + c->position + forward;
    simd_float3 ray_direction = image_point - c->position;
    
    ray.origin = c->position;
    ray.direction = ray_direction;
    
    return ray;
}

simd_float3 calcCameraRayDirection(simd_float3 right, simd_float3 up, simd_float3 forward, int i) {
    return simd_normalize(forward + (AA(i) * right) + (up * BB(i)));
}

void update_camera(Camera *c, float dt, SDL_bool*  keyboard_keys) {
    //Camera Movement
    simd_float3 forward = camera_Forward(c);
    simd_float3 right = camera_Right(c);
    simd_float3 up = camera_Up(c);
    
    if (keyboard_keys[SDL_SCANCODE_W]) {
        move_camera(c, forward, c->speed * dt);
    }
    else if (keyboard_keys[SDL_SCANCODE_S]) {
        move_camera(c, forward, -c->speed * dt);
    }
    if (keyboard_keys[SDL_SCANCODE_A]) {
        move_camera(c, right, -c->speed * dt);
    }
    else if (keyboard_keys[SDL_SCANCODE_D]){
        move_camera(c, right, c->speed * dt);
    }
    if (keyboard_keys[SDL_SCANCODE_R]) {
        move_camera(c, up, c->speed * dt);
    }
    else if (keyboard_keys[SDL_SCANCODE_F]){
        move_camera(c, up, -c->speed * dt);
    }
    
    //Camera Rotation
    if (keyboard_keys[SDL_SCANCODE_DOWN]) {
        rotate_camera(c, right, c->sensitivity * dt);
    }
    else if (keyboard_keys[SDL_SCANCODE_UP]) {
        rotate_camera(c, right, -c->sensitivity * dt);
    }
    if (keyboard_keys[SDL_SCANCODE_RIGHT]) {
        rotate_camera(c, up, c->sensitivity * dt);
    }
    else if (keyboard_keys[SDL_SCANCODE_LEFT]){
        rotate_camera(c, up, -c->sensitivity * dt);
    }
    if (keyboard_keys[SDL_SCANCODE_E]) {
        rotate_camera(c, forward, c->sensitivity * dt);
    }
    else if (keyboard_keys[SDL_SCANCODE_Q]){
        rotate_camera(c, forward, -c->sensitivity * dt);
    }
}

#endif /* Camera_h */

