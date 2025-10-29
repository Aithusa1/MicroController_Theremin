#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "bepaal_toonhoogte.h"
#include "bepaal_volume.h"

extern volatile uint8_t volume;
extern volatile uint8_t sound_enabled;

volatile uint16_t echo_time = 0;
volatile uint8_t measuring = 0;
volatile uint8_t buzzer_enabled = 0;

void setup_timer0_frequency(void) {
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS02);
    TIMSK0 = (1 << OCIE0A);
    OCR0A = (F_CPU / (256 * 800 * 2)) - 1;
}

ISR(TIMER0_COMPA_vect) {
    buzzer_enabled = !buzzer_enabled;
    
    if (sound_enabled && volume > 10 && buzzer_enabled) {
        TCCR2A |= (1 << COM2B1);
    } else {
        TCCR2A &= ~(1 << COM2B1);
    }
}

void update_freq(uint16_t freq) {
    if (freq < 230) freq = 230;
    if (freq > 1400) freq = 1400;
    
    uint32_t ocr = (16000000UL / (2UL * 256UL * freq)) - 1;
    
    if (ocr > 255) ocr = 255;
    if (ocr < 1) ocr = 1;
    
    OCR0A = ocr;
}

void smooth_freq(volatile uint16_t *current, uint16_t target) {
    int16_t diff = target - *current;
    
    if (diff > 30) {
        *current += 30;
    } else if (diff > 10) {
        *current += 10;
    } else if (diff > 0) {
        *current += 1;
    } else if (diff < -30) {
        *current -= 30;
    } else if (diff < -10) {
        *current -= 10;
    } else if (diff < 0) {
        *current -= 1;
    }
    
    update_freq(*current);
}

void setup_timer1_sensor(void) {
    TCCR1A = 0;
    TCCR1B = (1 << ICES1) | (1 << CS11);
    TIMSK1 = (1 << ICIE1);
}

ISR(TIMER1_CAPT_vect) {
    if (!measuring) {
        TCNT1 = 0;
        TCCR1B &= ~(1 << ICES1);
        measuring = 1;
    } else {
        echo_time = ICR1;
        TCCR1B |= (1 << ICES1);
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
    if (dist < 2) return 1400;
    if (dist > 65) return 230;
    return 1400 - ((dist - 2) * 1170) / 63;
}