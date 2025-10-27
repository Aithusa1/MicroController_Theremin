#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t volume = 128;
volatile uint16_t target_freq = 200;
volatile uint16_t current_freq = 200;
volatile uint8_t sound_enabled = 1;

#define TRIG_PIN PB1
#define ECHO_PIN PB0
#define BUZZER_PIN PD3

void setup_timer2_sound(void) {
    DDRD |= (1 << BUZZER_PIN);
    TCCR2A = (1 << WGM21);
    TCCR2B = (1 << CS22) | (1 << CS20);
    OCR2A = (F_CPU / (128 * 500)) - 1;
    TIMSK2 = (1 << OCIE2A);
}

ISR(TIMER2_COMPA_vect) {
    if (sound_enabled && volume > 10) {
        PORTD ^= (1 << BUZZER_PIN);
    } else {
        PORTD &= ~(1 << BUZZER_PIN);
    }
}

void update_freq(uint16_t *freq) {
    if (*freq < 50) *freq = 50;
    if (*freq > 2000) *freq = 2000;
    uint32_t ocr = (F_CPU / (128UL * (*freq))) - 1;
    if (ocr > 255) ocr = 255;
    OCR2A = ocr;
}

void smooth_freq(uint16_t *current, uint16_t target) {
    int16_t diff = target - *current;
    if (diff > 0) *current += diff / 8;
    else if (diff < 0) *current += diff / 8;
    update_freq(current);
}

void setup_adc(void) {
    ADMUX = (1 << REFS0) | (1 << ADLAR);
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

ISR(ADC_vect) {
    volume = ADCH;
    sound_enabled = (volume > 10);
}

void setup_timer1_sensor(void) {
    TCCR1A = 0;
    TCCR1B = (1 << ICES1) | (1 << CS11);
    TIMSK1 = (1 << ICIE1);
}

volatile uint16_t echo_time = 0;
volatile uint8_t measuring = 0;

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
    if (dist < 2 || dist > 65) return 150;
    return 2000 - ((dist - 2) * 1900) / 63;
}

int main(void) {
    DDRB |= (1 << TRIG_PIN);
    DDRB &= ~(1 << ECHO_PIN);
    
    setup_adc();
    setup_timer1_sensor();
    setup_timer2_sound();
    sei();
    
    while (1) {
        uint16_t dist = read_distance();
        target_freq = dist_to_freq(dist);
        smooth_freq(&current_freq, target_freq);
        _delay_ms(50);
    }
}