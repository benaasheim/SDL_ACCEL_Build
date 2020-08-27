//
//  main.c
//  SDL_ACCEL13
//
//  Created by Ben on 8/26/20.
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
#include "InputHandler.h"

//Globals
Scene* scene;
Camera* camera;
Uint32 * pixels;
SDL_PixelFormat* SF;

void sub_display(void *arguments) {
    // Ray-tracing loop, for each pixel
    int* args = (int *)arguments;
    int start = *args;
    int end = args[1];
    
    Ray ray;
    ray.origin = camera->position;
    
    simd_float3 right = camera_Right(camera);
    simd_float3 up = camera_Up(camera);
    simd_float3 forward = camera_Forward(camera);
    
    simd_float3 col;
    int x, y;
    for (y = start; y < end; y++) {
        for (x = 0; x < window_width; x++) {
            ray.direction = simd_normalize(forward + (AA(x) * right) + (up * BB(y)));
            col = traceColor(ray, scene, 0) * 255;
            pixels[y * window_width + x] = SDL_MapRGB(SF, col.x, col.y, col.z);
        }
    }
}

void handle_threads(pthread_t* thread_lis, int* thread_args) {
    int i;
    //create all the threads one by one
    for (i = 0; i < threads; i++) {
        pthread_create(&thread_lis[i], NULL, sub_display, &thread_args[i*2]);
    }
    //wait for each thread to complete
    for (i = 0; i < threads; i++) {
        pthread_join(thread_lis[i], NULL);
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    unsigned int frames = 0;
    pthread_t thread_lis[threads];
    int thread_args[threads * 2];
    
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
    Uint32 pix[window_height][window_width];
    pixels = pix;
    
    //init
    Tracer tracer = newTracer(init_Scene3);
    camera = &tracer.camera;
    scene = &tracer.scene;
    
    initKeyboardKeys();
    
    int startind = 0;
    int currind  = 0;
    int done = 0;
    while (!done) {
        thread_args[currind] = startind;
        startind +=  y_placeholder;
        thread_args[currind+1] = startind;
        if (y_placeholder + startind >= window_height || currind >= threads*2) {
            done = 1;
        }
        else {
            currind += 2;
        }
    }
    thread_args[currind-1] = window_height;
    
    // annimation loop
    int close = 0;
    
    double time_taken;
    
    clock_t t;
    int x, y;
    
    //get screen and screen format pointers
    SDL_Surface* screen = SDL_GetWindowSurface(win);
    SDL_PixelFormat* screenformat = screen->format;
    SF = screenformat;
    
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
                    keyboard_keys[event.key.keysym.scancode] = 1;
                    break;
                case SDL_KEYUP:
                    keyboard_keys[event.key.keysym.scancode] = 0;
                    break;
            }
        }
        
        
        //cast&trace rays
        handle_threads(thread_lis, thread_args);
        
        
        
        //draw rays to screen
        for (y = 0; y < window_height; y++) {
            for (x = 0; x < window_width; x++) {
                rect.x = x * resolution;
                rect.y = y * resolution;
                SDL_FillRect(screen, &rect, pix[y][x]);
            }
        }
        SDL_UpdateWindowSurface(win);
        
        
        //update tracer stuff
        t = clock() - t;
        time_taken = ((double)t)/CLOCKS_PER_SEC;
        update_camera(camera, time_taken, keyboard_keys);
        update_Scene(scene, time_taken);
        
        
        //update frames
        frames++;
        printf("Frames: %d\n", frames);
    }
    
    // destroy window
    SDL_Quit();
    SDL_DestroyWindow(win);
    
    return 0;
}

