//
//  Settings.h
//  SDL_ACCEL13
//
//  Created by Ben on 8/26/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef Settings_h
#define Settings_h

// Control variables for the different features
#define window_width 700
#define window_height 700
#define aspect_ratio (window_width / window_height)
#define resolution 1
#define dheight (window_height * resolution)
#define dwidth (window_width * resolution)
#define epsilon 1e-3f
#define tthreads 8
#define threads (tthreads * tthreads)
#define max_recursion 5
#define camera_speed 0.5f
#define camera_sensitivity 0.1f
#define total_pixels (window_height * window_width)
#define step_placeholder (total_pixels / threads)
#define y_placeholder (window_height / threads)

#endif /* Settings_h */

