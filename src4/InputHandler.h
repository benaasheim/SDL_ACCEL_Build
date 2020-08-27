//
//  InputHandler.h
//  SDL_ACCEL13
//
//  Created by Ben on 8/27/20.
//  Copyright © 2020 Ben. All rights reserved.
//

#ifndef InputHandler_h
#define InputHandler_h

#define num_keys 255
unsigned char keyboard_keys[num_keys];

void initKeyboardKeys() {
    for (int i = 0; i < num_keys; i++) {
        keyboard_keys[i] = 0;
    }
}

#endif /* InputHandler_h */

