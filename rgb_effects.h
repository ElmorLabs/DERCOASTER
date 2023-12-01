/* 
 * File:   rgb_effects.h
 * Author: JonSandström
 *
 * Created on March 16, 2023, 12:36 PM
 */

#ifndef RGB_EFFECTS_H
#define	RGB_EFFECTS_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
    // Define number of effects
    #define NUM_EFFECTS 10
    
    void rgb_effects_loop();
    void rgb_set_effect(uint8_t effect);
    void rgb_get_effect(uint8_t* effect);
    void rgb_set_color(uint8_t r, uint8_t g, uint8_t b);
    void rgb_get_color(uint8_t* r, uint8_t* g, uint8_t* b);
    void rainbow_get_values(uint8_t hue, uint8_t* r, uint8_t* g, uint8_t* b);

#ifdef	__cplusplus
}
#endif

#endif	/* RGB_EFFECTS_H */

