#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "bepaal_volume.h"

volatile uint8_t volume = 128;

void adc_init(void){
    ADMUX  = (1<<REFS0)|(1<<ADLAR);           // AVCC ref, ADC0, left adjust
    ADCSRA = (1<<ADEN)|(1<<ADSC)|(1<<ADATE)|(1<<ADIE)
            |(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
    sei();
}

ISR(ADC_vect){ 
    volume = ADCH;                            // 0–255
}

void timer2_init(void){
    DDRD |= (1<<PD3);                         // PD3 = OC2B output to buzzer
    // Fast PWM on OC2B, non-inverting
    TCCR2A = (1<<COM2B1)|(1<<WGM21)|(1<<WGM20);
    // Prescaler 64 → about 1 kHz PWM carrier
    TCCR2B = (1<<CS21)|(1<<CS20);
    OCR2B  = 128;
}