#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "bepaal_toonhoogte.h"
#include "bepaal_volume.h"

// Externe variabelen
extern volatile uint8_t volume;
extern volatile uint8_t sound_enabled;

volatile uint16_t echo_time = 0;
volatile uint8_t measuring = 0;
volatile uint8_t buzzer_enabled = 0;

void setup_timer0_frequency(void) {
    // Timer0 voor frequentie generatie - CTC mode volgens ontwerp
    TCCR0A = (1 << WGM01); // CTC mode
    TCCR0B = (1 << CS02); // Prescaler 256 volgens ontwerp
    TIMSK0 = (1 << OCIE0A); // Compare match A interrupt
    
    // Start frequentie op 800Hz (midden in bereik)
    OCR0A = (F_CPU / (256 * 800 * 2)) - 1;
}

ISR(TIMER0_COMPA_vect) {
    // Toggle de buzzer enable state - dit moduleert de PWM output
    buzzer_enabled = !buzzer_enabled;
    
    if (sound_enabled && volume > 10 && buzzer_enabled) {
        // Zet PWM output aan
        TCCR2A |= (1 << COM2B1);
    } else {
        // Zet PWM output uit
        TCCR2A &= ~(1 << COM2B1);
    }
}

void update_freq(uint16_t freq) {
    // Begrens frequentie volgens technisch ontwerp: 230Hz - 1400Hz
    if (freq < 230) freq = 230;
    if (freq > 1400) freq = 1400;
    
    // CORRECTE BEREKENING volgens ontwerp:
    // f = F_CPU / (2 * N * (1 + OCR0A)) 
    uint32_t ocr = (16000000UL / (2UL * 256UL * freq)) - 1;
    
    if (ocr > 255) ocr = 255;
    if (ocr < 1) ocr = 1;
    
    OCR0A = ocr;
}

// Snellere frequentie verandering
void smooth_freq(volatile uint16_t *current, uint16_t target) {
    int16_t diff = target - *current;
    
    // Grotere stappen voor snellere respons
    if (diff > 30) {
        *current += 30;  // Snelle toename
    } else if (diff > 10) {
        *current += 10;   // Medium toename
    } else if (diff > 0) {
        *current += 1;   // Fijnafstelling
    } else if (diff < -30) {
        *current -= 30;  // Snelle afname
    } else if (diff < -10) {
        *current -= 10;   // Medium afname
    } else if (diff < 0) {
        *current -= 1;   // Fijnafstelling
    }
    
    update_freq(*current);
}

void setup_timer1_sensor(void) {
    TCCR1A = 0; // Normal mode
    TCCR1B = (1 << ICES1) | (1 << CS11); // Rising edge, prescaler 8
    TIMSK1 = (1 << ICIE1); // Input capture interrupt
}

ISR(TIMER1_CAPT_vect) {
    if (!measuring) {
        TCNT1 = 0;
        TCCR1B &= ~(1 << ICES1); // Falling edge
        measuring = 1;
    } else {
        echo_time = ICR1;
        TCCR1B |= (1 << ICES1); // Rising edge
        measuring = 0;
    }
}

uint16_t read_distance(void) {
    PORTB |= (1 << TRIG_PIN);
    _delay_us(10);
    PORTB &= ~(1 << TRIG_PIN);
    _delay_ms(50);
    
    if (!measuring && echo_time > 0) {
        uint16_t dist = (echo_time * 2) / 58;
        echo_time = 0;
        return (dist < 2 || dist > 400) ? 0 : dist;
    }
    return 0;
}

uint16_t dist_to_freq(uint16_t dist) {
    // Volgens technisch ontwerp: 230Hz - 1400Hz
    // Bij afstand < 2cm: 1400Hz (hoogste toon)
    // Bij afstand > 65cm: 230Hz (laagste toon)
    if (dist < 2) return 1400;
    if (dist > 65) return 230;
    
    // Lineaire mapping: 2cm -> 1400Hz, 65cm -> 230Hz
    // f = 1400 - ((dist - 2) * (1400 - 230)) / (65 - 2)
    return 1400 - ((dist - 2) * 1170) / 63;
}