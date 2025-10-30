#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "twi.h"
#include "weergeven_afstand_hoogte.h"
#include "bepaal_toonhoogte.h"
#include "bepaal_volume.h"
#include "filter_toonhoogte.h"
#include "weergeven_filter.h"
#include "filter_buttons.h"

extern volatile uint8_t volume;
extern volatile uint8_t sound_enabled;

volatile uint16_t target_freq = 800;
volatile uint16_t current_freq = 800;

void display_info(uint16_t dist, uint16_t freq) {
    lcd_command(0x80);
    char buffer[16];
    
    if (dist > 0) {
        snprintf(buffer, sizeof(buffer), "Dist:%3dcm", dist);
    } else {
        snprintf(buffer, sizeof(buffer), "No signal");
    }
    lcd_print(buffer);
    
    lcd_command(0xC0);
    snprintf(buffer, sizeof(buffer), "%4dHz    %3d", freq, volume);
    lcd_print(buffer);
}

int main(void) {
    DDRB |= (1 << TRIG_PIN);
    DDRB &= ~(1 << ECHO_PIN);
    
    TWI_Init();
    filter_buttons_init();
    _delay_ms(1000);
    lcd_init();
    filter_init();
    
    setup_adc();
    setup_timer2_volume_pwm();
    setup_timer0_frequency();
    setup_timer1_sensor();
    
    lcd_command(0x01);
    lcd_print("Theremin Ready");
    _delay_ms(1000);
    
    display_filter_size(filter_buttons_get_size());
    
    sei();
    
    while(1) {
        uint16_t dist = read_distance();
        target_freq = dist_to_freq(dist);
        
        filter_add_value(target_freq);
        uint16_t filtered_freq = filter_get_median();
        
        smooth_freq(&current_freq, filtered_freq);
        
        OCR2B = volume;
        
        display_info(dist, current_freq);
        
        filter_buttons_update();
        
        _delay_ms(15);
    }
}