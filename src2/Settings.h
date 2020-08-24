//
//  Settings.h
//  SDL_ACCEL9
//
//  Created by Ben on 8/11/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Settings_h
#define Settings_h

// Control variables for the different features
#define window_width 400
#define window_height 200
#define aspect_ratio (float) (window_width / window_height)
#define resolution 1
#define dheight (window_height * resolution)
#define dwidth (window_width * resolution)
#define epsilon 1e-3f
#define tthreads 8
#define threads (tthreads * tthreads)
#define max_recursion 5
#define camera_speed 0.5
#define camera_sensitivity 5
#define total_pixels (window_height * window_width)
#define step_placeholder (total_pixels / threads);

#endif /* Settings_h */

