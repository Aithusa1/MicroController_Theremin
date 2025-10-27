#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "twi.h"
#include "weergeven_afstand_hoogte.h"

// Global variables
volatile uint8_t volume = 128;
volatile uint16_t target_freq = 500;
volatile uint16_t current_freq = 500;
volatile uint8_t sound_enabled = 1;

#define TRIG_PIN PB1
#define ECHO_PIN PB0
#define BUZZER_PIN PD3

// Externe functies uit bepaal_toonhoogte.c
void setup_timer2_sound(void);
void setup_timer1_sensor(void);
void update_freq(uint16_t *freq);
void smooth_freq(uint16_t *current, uint16_t target);
uint16_t read_distance(void);
uint16_t dist_to_freq(uint16_t dist);

void setup_adc(void) {
    ADMUX = (1 << REFS0) | (1 << ADLAR);
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

ISR(ADC_vect) {
    volume = ADCH;
    sound_enabled = (volume > 10);
}

// Functie om info op LCD te tonen
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
    
    setup_adc();
    setup_timer1_sensor();
    setup_timer2_sound();
    
    // Start bericht
    lcd_command(0x01);
    lcd_print("Theremin Ready");
    _delay_ms(1000);
    
    sei();
    
    while (1) {
        uint16_t dist = read_distance();
        target_freq = dist_to_freq(dist);
        smooth_freq(&current_freq, target_freq);
        
        // Toon info op LCD
        display_info(dist, current_freq);
        
        _delay_ms(50);
    }
}