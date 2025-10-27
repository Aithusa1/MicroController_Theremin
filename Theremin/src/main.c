#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "twi.h"
#include "weergeven_afstand_hoogte.h"
#include "bepaal_toonhoogte.h"
#include "bepaal_volume.h"

// Global variables - gedefinieerd in volume.c
extern volatile uint8_t volume;
extern volatile uint8_t sound_enabled;

volatile uint16_t target_freq = 800;  // Start in midden bereik
volatile uint16_t current_freq = 800;

void display_info(uint16_t dist, uint16_t freq) {
    lcd_command(0x80); // Eerste regel
    char buffer[16];
    
    if (dist > 0) {
        snprintf(buffer, sizeof(buffer), "Dist:%3dcm ", dist);
    } else {
        snprintf(buffer, sizeof(buffer), "No signal   ");
    }
    lcd_print(buffer);
    
    lcd_command(0xC0); // Tweede regel
    snprintf(buffer, sizeof(buffer), "F:%4dHz V:%3d", freq, volume);
    lcd_print(buffer);
}

int main(void) {
    // Pin configuratie
    DDRB |= (1 << TRIG_PIN);
    DDRB &= ~(1 << ECHO_PIN);
    
    // Initialisatie
    TWI_Init();
    _delay_ms(1000);
    lcd_init();
    
    // Hardware initialisatie VOLGENS TECHNISCH ONTWERP:
    setup_adc();                    // Volume ADC
    setup_timer2_volume_pwm();      // Timer2: Fast PWM voor volume
    setup_timer0_frequency();       // Timer0: CTC mode voor frequentie
    setup_timer1_sensor();          // Timer1: Sensor meting
    
    // Start bericht
    lcd_command(0x01);
    lcd_print("Theremin Ready");
    _delay_ms(1000);
    
    // Global interrupts inschakelen
    sei();
    
    // Hoofdloop - SNEL updates
    while(1) {
        uint16_t dist = read_distance();
        target_freq = dist_to_freq(dist);
        smooth_freq(&current_freq, target_freq);
        
        // Update PWM duty cycle op basis van volume
        OCR2B = volume;
        
        // Display info
        display_info(dist, current_freq);
        
        _delay_ms(15);  // Zeer korte delay voor snelle respons
    }
}