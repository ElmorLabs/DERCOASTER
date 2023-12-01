/*
 * File:   main.c
 * Author: JonSandström
 *
 * Created on November 10, 2022, 10:33 AM
 */

#include <xc.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#include "ws2812b.h"
#include "light_ws2812.h"
#include "rgb_effects.h"

#define EEPROM_OFFSET_RGB_EFFECT 0
#define EEPROM_OFFSET_RGB_COLOR_R 1
#define EEPROM_OFFSET_RGB_COLOR_G 2
#define EEPROM_OFFSET_RGB_COLOR_B 3

#define BUTTON_LONG_PRESS_TIMEOUT 25 // 25*10 = 250ms

int main(void) {
    
    // Clock 8MHz
    CLKPR = 0x80;
    CLKPR = 0x00;
    
    // Pin PB2 = DOUT
    DDRB |= _BV(PB2); // output
    PORTB |= _BV(PB2); // output high

    // Pin PA3 = Push Button
    DDRA &= ~(_BV(PA3)); // input
    PORTA |= _BV(PA3); // internal pull up
    
    // Read effect from EEPROM
    uint8_t rgb_effect = eeprom_read_byte((const uint8_t *)(EEPROM_OFFSET_RGB_EFFECT));
    uint8_t rgb_color_r = eeprom_read_byte((const uint8_t *)(EEPROM_OFFSET_RGB_COLOR_R));
    uint8_t rgb_color_g = eeprom_read_byte((const uint8_t *)(EEPROM_OFFSET_RGB_COLOR_G));
    uint8_t rgb_color_b = eeprom_read_byte((const uint8_t *)(EEPROM_OFFSET_RGB_COLOR_B));
    
    // Check button state
    uint8_t button_pressed = (PINA & (1 << PINA3)) == 0;
    int button_timeout_cnt = 0;
    
    // If rgb_effect is above the number of effects, or button is pressed, restore defaults (pulsating der8auer yellow)
    if(rgb_effect >= NUM_EFFECTS || button_pressed) {
        // Get default rgb settings
        rgb_get_effect(&rgb_effect);
        rgb_get_color(&rgb_color_r, &rgb_color_g, &rgb_color_b);
        
        // Wait for button press release
        while((PINA & (1 << PINA3)) == 0);
        
        // Prevent additional presses for 200ms
        button_timeout_cnt = 20;
    } else {
        // Set stored rgb effect values
        rgb_set_effect(rgb_effect);
        rgb_set_color(rgb_color_r, rgb_color_g, rgb_color_b);
    }
    
    int button_press_cnt = 0;
    
	/* loop */
	while (1) {
     
        // Check button status
        button_pressed = (PINA & (1 << PINA3)) == 0;
        
        if(button_pressed) {
            
            if(button_timeout_cnt) {
                button_timeout_cnt--;
            } else {
            
                // Check if long press
                if(button_press_cnt >= BUTTON_LONG_PRESS_TIMEOUT) {
                    // Change color
                    static uint8_t hue = 0;
                    rainbow_get_values(hue, &rgb_color_r, &rgb_color_g, &rgb_color_b);
                    rgb_set_color(rgb_color_r, rgb_color_g, rgb_color_b);

                    hue += 1;

                } else {
                    button_press_cnt++;
                }
            }
        } else if(button_press_cnt) {
            
            // Get current rgb settings
            rgb_get_effect(&rgb_effect);
            rgb_get_color(&rgb_color_r, &rgb_color_g, &rgb_color_b);
            
            // Check if stopped long press
            if(button_press_cnt >= BUTTON_LONG_PRESS_TIMEOUT) {
                // Change color
                rgb_set_color(rgb_color_r, rgb_color_g, rgb_color_b);
            }
            // Or if normal press
            else {
                // Change effect
                rgb_effect++;
                rgb_set_effect(rgb_effect);
            }
            
            // Update eeprom
            eeprom_update_byte((uint8_t *)(EEPROM_OFFSET_RGB_EFFECT), rgb_effect);
            eeprom_update_byte((uint8_t *)(EEPROM_OFFSET_RGB_COLOR_R), rgb_color_r);
            eeprom_update_byte((uint8_t *)(EEPROM_OFFSET_RGB_COLOR_G), rgb_color_g);
            eeprom_update_byte((uint8_t *)(EEPROM_OFFSET_RGB_COLOR_B), rgb_color_b);
            
            // Reset button press counter
            button_press_cnt = 0;
            
            // Prevent additional presses for 100ms
            button_timeout_cnt = 10;
        }
        
        // RGB effect update function
        rgb_effects_loop();
        
        // Loop delay
        _delay_ms(10);
        
	}
    
    return 0;
}
