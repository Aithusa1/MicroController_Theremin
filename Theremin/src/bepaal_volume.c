#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "bepaal_volume.h"

// Global variables voor volume
volatile uint8_t volume = 128;        // Start op halve volume (0-255)
volatile uint8_t sound_enabled = 1;   // Geluid aan/uit

void setup_adc(void) {
    // ADC initialisatie voor volume potmeter op ADC0 (PC0)
    ADMUX = (1 << REFS0) | (1 << ADLAR);  // AVCC reference, left adjust result
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE) | 
             (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler 128
}

ISR(ADC_vect) {
    volume = ADCH;  // Lees ADC waarde (0-255)
    sound_enabled = (volume > 10);
}

void setup_timer2_volume_pwm(void) {
    DDRD |= (1 << PD3);  // PD3 = OC2B output voor buzzer volume PWM
    
    // Fast PWM mode voor Timer2 - volgens technisch ontwerp
    TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20); // Non-inverting PWM, Fast PWM
    TCCR2B = (1 << CS20); // No prescaling (snelste PWM) - volgens ontwerp
    
    // Stel duty cycle in op basis van volume
    OCR2B = volume;
}