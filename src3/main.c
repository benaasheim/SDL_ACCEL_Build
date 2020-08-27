//
//  main.c
//  SDL_ACCEL12
//
//  Created by Ben on 8/23/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

#include <stdio.h>
#include "Ray.h"
#include "Light.h"
#include "Settings.h"
#include "Materials.h"
#include "Scene.h"
#include "Tracer.h"


//Globals
Scene* scene;
Camera* camera;
simd_float3* pixels;
//SDL_Surface* screen;
//SDL_PixelFormat* screenformat;

void sub_display(void *arguments) {
    // Ray-tracing loop, for each pixel
    
    int start = *((int *)arguments);
    int end = start + step_placeholder;
    
    Ray ray;
    ray.origin = camera->position;
    
    simd_float3 right = camera_Right(camera);
    simd_float3 up = camera_Up(camera);
    simd_float3 forward = camera_Forward(camera);
    
    SDL_Rect rect;
    rect.h = resolution;
    rect.w = resolution;
//    for (int i = start; i < end; i++) {
//        ray.direction = forward + (AA(i) * right) + (up * BB(i));
//        ray.direction = simd_normalize(ray.direction);
//        pixels[i] = traceColor(ray, scene, 0) * 255;
//    }
}

void sub_display2(void *arguments) {
    // Ray-tracing loop, for each pixel
    
    int start = *((int *)arguments);
    int end = start + y_placeholder;
    
    Ray ray;
    ray.origin = camera->position;
    
    simd_float3 right = camera_Right(camera);
    simd_float3 up = camera_Up(camera);
    simd_float3 forward = camera_Forward(camera);
    
//    simd_float3 col;
    int x, y;
//    SDL_PixelFormat* screenformat = screen->format;
    for (y = start; y < end; y++) {
        for (x = 0; x < window_width; x++) {
            ray.direction = calcCameraRayDirection(right, up, forward, y * window_width + x);
            pixels[y * window_width + x] = traceColor(ray, scene, 0) * 255;
        }
    }
}

void handle_threads(pthread_t* thread_lis, int* thread_args) {
    int i;
    //create all the threads one by one
    for (i = 0; i < threads; i++) {
        pthread_create(&thread_lis[i], NULL, sub_display2, &thread_args[i]);
    }
    //wait for each thread to complete
    for (i = 0; i < threads; i++) {
        pthread_join(thread_lis[i], NULL);
    }
}

void handle_threads2(pthread_t* thread_lis, int* thread_args) {
    int i;
    //create all the threads one by one
    for (i = 0; i < threads; i++) {
        pthread_create(&thread_lis[i], NULL, sub_display2, &thread_args[i]);
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("HERE");
    unsigned int frames = 0;
    pthread_t thread_lis[threads];
    int thread_args[threads];
    
    // retutns zero on success else non-zero
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    
    // creates a window
    SDL_Window* win = SDL_CreateWindow("GAME",
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,
                                        dwidth,
                                        dheight,
                                        0);
    // retutns zero on success else non-zero
    if (win == NULL) {
        printf("error initializing WINDOW: %s\n", SDL_GetError());
    }
    
    SDL_Rect rect;
    rect.h = resolution;
    rect.w = resolution;
    simd_float3 pix[window_height][window_width];
    pixels = pix;
//    pixels = (simd_float3**)malloc(
//                                    sizeof(simd_float3)*total_pixels
//                                    );
    simd_float3 col;
    
    //init
    Tracer tracer = newTracer(init_Scene3);
    camera = &tracer.camera;
    scene = &tracer.scene;
    
    //create all the thread arguements one by one
    for (int i = 0; i < threads; i++) {
        thread_args[i] = i * y_placeholder;
    }
    
//    for (int i = 0; i < total_pixels; i++) {
//        pixels[i/window_width][i%window_width] = simd_make_float3(0, 0, 0);
////        pixels[i] = simd_make_float3(0, 0, 0);
//    }
    
    // annimation loop
    int close = 0;
    
    double time_taken;
    
    clock_t t;
    int x, y;
    
    //get screen and screen format pointers
    SDL_Surface* screen = SDL_GetWindowSurface(win);
    SDL_PixelFormat* screenformat = screen->format;
    
    //Set screen blank at first
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 255, 255));
    SDL_UpdateWindowSurface(win);

    while (!close) {
        
        //start clock
        t = clock();
        
        
        // Events mangement
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

                case SDL_QUIT:
                    // handling of close button
                    close = 1;
                    break;
                case SDL_KEYDOWN:
                    // keyboard API for key pressed
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        close = 1;
                        break;
                    }
                    tracer.keyboard_keys[event.key.keysym.scancode] = SDL_TRUE;
                    break;
                case SDL_KEYUP:
                    tracer.keyboard_keys[event.key.keysym.scancode] = SDL_FALSE;
                    break;
            }
        }
        
        
        //cast&trace rays
        handle_threads(thread_lis, thread_args);
        
        
//        //draw rays to screen
        for (y = 0; y < window_height; y++) {
            for (x = 0; x < window_width; x++) {
                col = pix[y][x];
                rect.x = x * resolution;
                rect.y = y * resolution;
                SDL_FillRect(screen, &rect, SDL_MapRGB(screenformat, col.x, col.y, col.z));
            }
        }
        SDL_UpdateWindowSurface(win);
        
        
        //update tracer stuff
        t = clock() - t;
        time_taken = ((double)t)/CLOCKS_PER_SEC;
        update_camera(camera, time_taken, tracer.keyboard_keys);
//        update_Scene(scene, time_taken);
        
        
        //update frames
        frames++;
        printf("Frames: %d\n", frames);
    }
    
    // destroy window
    SDL_Quit();
    SDL_DestroyWindow(win);
    
    return 0;
}

