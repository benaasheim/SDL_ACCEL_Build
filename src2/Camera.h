//
//  Camera.h
//  SDL_ACCEL9
//
//  Created by Ben on 8/11/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Camera_h
#define Camera_h

#include <stdio.h>
#include "Ray.h"
#include "Quaternion.h"
#include "Settings.h"

#include <SDL2/SDL.h>

#define AA(i) (((i%window_width) - window_width * 0.5)/window_width*aspect_ratio)
#define BB(i) ((window_height * 0.5 - (i/window_width))/window_height)

//typedef struct {
//    vec position;
//    vec rot;
//    float speed;
//    float sensitivity;
//} Camera;

struct __attribute__ ((__packed__)) Camera {
    vec position;
    vec rot;
    float speed;
    float sensitivity;
};

typedef struct Camera Camera;

Camera newCamera(vec v, float sp, float se) {
    Camera camera;
    camera.position = v;
    vec r = {0, 1, 0, 0};
    camera.rot = r;
    camera.speed = sp;
    camera.sensitivity = se;
    return camera;
}

void move_camera(Camera* c, float * restrict dir, float amount) {
    mulfv(amount, dir, dir);
    addVec(&c->position, dir, &c->position);
}

void rotate_camera(Camera* c, float * restrict axis, float theta) {
    QuaternionVTSelf(axis, theta);
    mulqq(axis, &c->rot, &c->rot);
    normalizeQSelf(&c->rot);
}

void camera_Forward(Camera* c, float * restrict a) {
    ForwardVector(&c->rot, a);
}

void camera_Right(Camera* c, float * restrict a) {
    RightVector(&c->rot, a);
}

void camera_Up(Camera* c, float * restrict a) {
    UpVector(&c->rot, a);
}

Ray calcCameraRay(Camera* c, int i) {
    Ray ray;

//    ray.origin = camera_Right_Vec(c);
    camera_Right(c, &ray.origin);
    mulfv(AA(i), &ray.origin, &ray.origin);

//    ray.direction = camera_Up_Vec(c);
    camera_Up(c, &ray.direction);
    mulfv(BB(i), &ray.direction, &ray.direction);

    addVec(&ray.origin, &ray.direction, &ray.origin);

//    ray.direction = camera_Forward_Vec(c);
    camera_Forward(c, &ray.direction);
    addVec(&ray.origin, &ray.direction, &ray.direction);

    normalize(&ray.direction, &ray.direction);

    ray.origin = c->position;

    return ray;
}

Ray calcCameraRay2(Camera* c, int w, int h, float ar, int i) {
    Ray ray;
    
    int x, y;
    x = i % window_width;
    y = i / window_height;
    
    float x_norm, y_norm;
    x_norm = (x - w * 0.5f) / w * ar;
    y_norm = (h * 0.5f - y) / h;
    
    vec forward, up, right, image_point, ray_direction;
    camera_Forward(c, &forward);
    camera_Up(c, &up);
    camera_Right(c, &right);
    
    mulfvSelf(x_norm, &right);
    mulfvSelf(y_norm, &up);
    
    vec temp;
    addVec(&right, &up, &temp);
    addVec(&c->position, &forward, &forward);
    addVec(&forward, &temp, &image_point);
    
    subVec(&image_point, &c->position, &ray_direction);
    
    ray.direction = ray_direction;
    ray.origin = c->position;
    return ray;
}

Ray calcCameraRay3(Camera* c, int w, int h, float ar, int x, int y) {
    Ray ray;
    
    float x_norm, y_norm;
    x_norm = (x - w * 0.5f) / w * ar;
    y_norm = (h * 0.5f - y) / h;
    
    vec forward, up, right, image_point, ray_direction;
    camera_Forward(c, &forward);
    camera_Up(c, &up);
    camera_Right(c, &right);
    
    mulfvSelf(x_norm, &right);
    mulfvSelf(y_norm, &up);
    
    vec temp;
    addVec(&right, &up, &temp);
    addVec(&c->position, &forward, &forward);
    addVec(&forward, &temp, &image_point);
    
    subVec(&image_point, &c->position, &ray_direction);
    
    ray.direction = ray_direction;
    ray.origin = c->position;
    return ray;
}

void update_camera(Camera *c, float dt, SDL_bool*  keyboard_keys) {
    //Camera Movement
    vec forward, right, up;
    camera_Forward(c, &forward);
    camera_Right(c, &right);
    camera_Up(c, &up);
    
    if (keyboard_keys[SDL_SCANCODE_W]) {
        move_camera(c, &forward, c->speed * dt);
    }
    else if (keyboard_keys[SDL_SCANCODE_S]) {
        move_camera(c, &forward, -c->speed * dt);
    }
    if (keyboard_keys[SDL_SCANCODE_A]) {
        move_camera(c, &right, -c->speed * dt);
    }
    else if (keyboard_keys[SDL_SCANCODE_D]){
        move_camera(c, &right, c->speed * dt);
    }
    if (keyboard_keys[SDL_SCANCODE_R]) {
        move_camera(c, &up, c->speed * dt);
    }
    else if (keyboard_keys[SDL_SCANCODE_F]){
        move_camera(c, &up, -c->speed * dt);
    }
    
    //Camera Rotation
    if (keyboard_keys[SDL_SCANCODE_DOWN]) {
        rotate_camera(c, &right, c->sensitivity * dt);
    }
    else if (keyboard_keys[SDL_SCANCODE_UP]) {
        rotate_camera(c, &right, -c->sensitivity * dt);
    }
    if (keyboard_keys[SDL_SCANCODE_RIGHT]) {
        rotate_camera(c, &up, c->sensitivity * dt);
    }
    else if (keyboard_keys[SDL_SCANCODE_LEFT]){
        rotate_camera(c, &up, -c->sensitivity * dt);
    }
    if (keyboard_keys[SDL_SCANCODE_E]) {
        rotate_camera(c, &forward, c->sensitivity * dt);
    }
    else if (keyboard_keys[SDL_SCANCODE_Q]){
        rotate_camera(c, &forward, -c->sensitivity * dt);
    }
}

#endif /* Camera_h */

