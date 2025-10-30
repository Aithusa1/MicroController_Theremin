#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "bepaal_volume.h"

volatile uint8_t volume = 128;
volatile uint8_t sound_enabled = 1;

void setup_adc(void) {
    ADMUX = (1 << REFS0) | (1 << ADLAR);
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE) | 
             (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADCSRB = 0;
}

ISR(ADC_vect) {
    volume = ADCH;
    sound_enabled = (volume > 10);
}

void setup_timer2_volume_pwm(void) {
    DDRD |= (1 << PD3);
    TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
    TCCR2B = (1 << CS20);
    OCR2B = volume;
}