#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t volume = 128;
#define TRIG_PIN PB1
#define ECHO_PIN PB0
#define BUZZER_PIN PD3

volatile uint16_t echo_time = 0;
volatile uint8_t edge = 0;
volatile uint8_t distance_ready = 0;
volatile uint16_t distance_cm = 0;
volatile uint16_t current_freq = 200;
volatile uint16_t target_freq = 200;
volatile uint8_t sound_enabled = 1;

#define SMOOTHING_FACTOR 32

void adc_init(void){
    ADMUX  = (1<<REFS0)|(1<<ADLAR);
    ADCSRA = (1<<ADEN)|(1<<ADSC)|(1<<ADATE)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

ISR(ADC_vect){ 
    volume = ADCH;
    if (volume < 10) {
        sound_enabled = 0;
        PORTD &= ~(1 << BUZZER_PIN);
    } else {
        sound_enabled = 1;
    }
}

void setup_timer1_for_distance(void){
    TCCR1A = 0x00;
    TCCR1B = (1 << ICES1) | (1 << CS11);
    TIMSK1 = (1 << ICIE1);
}

void setup_timer2_for_sound(void) {
    DDRD |= (1 << BUZZER_PIN);
    TCCR2A = (1 << WGM21);
    TCCR2B = (1 << CS22) | (1 << CS20);
    OCR2A = (F_CPU / (128UL * 500)) - 1;
    TIMSK2 = (1 << OCIE2A);
}

ISR(TIMER2_COMPA_vect) {
    if (sound_enabled && volume > 10) {
        PORTD ^= (1 << BUZZER_PIN);
    } else {
        PORTD &= ~(1 << BUZZER_PIN);
    }
}

void update_frequency(uint16_t freq) {
    if (freq < 50) freq = 50;
    if (freq > 2000) freq = 2000;
    uint32_t ocr_value = (F_CPU / (128UL * freq)) - 1;
    if (ocr_value > 255) ocr_value = 255;
    if (ocr_value < 1) ocr_value = 1;
    OCR2A = ocr_value;
}

void smooth_frequency_update(void) {
    int16_t diff = target_freq - current_freq;
    if (diff == 0) return;
    
    if (diff > 0) {
        uint16_t step = diff / SMOOTHING_FACTOR;
        if (step < 2) step = 2;
        current_freq += step;
        if (current_freq > target_freq) current_freq = target_freq;
    } else {
        uint16_t step = (-diff) / SMOOTHING_FACTOR;
        if (step < 2) step = 2;
        current_freq -= step;
        if (current_freq < target_freq) current_freq = target_freq;
    }
    update_frequency(current_freq);
}

void trigger_sensor(void){
    PORTB |= (1 << TRIG_PIN);
    _delay_us(10);
    PORTB &= ~(1 << TRIG_PIN);
}

ISR(TIMER1_CAPT_vect){
    if (edge == 0){
        TCNT1 = 0;
        TCCR1B &= ~(1 << ICES1);
        edge = 1;
    } else {
        echo_time = ICR1;
        TCCR1B |= (1 << ICES1);
        edge = 0;
        distance_ready = 1;
    }
}

uint16_t afstand_meten(void){
    if (distance_ready){
        distance_ready = 0;
        uint32_t tijd_us = (uint32_t)echo_time * 2;
        distance_cm = tijd_us / 58;
        if (distance_cm > 400 || distance_cm < 2) distance_cm = 0;
    }
    return distance_cm;
}

uint16_t bereken_frequentie(uint16_t afstand){
    if (afstand < 2 || afstand > 65) return 150;
    uint16_t f = 2000 - ((afstand - 2) * (2000 - 100)) / 63;
    if (f < 100) f = 100;
    if (f > 2000) f = 2000;
    return f;
}

uint16_t smooth_afstand_meten(void) {
    static uint16_t distance_history[4] = {0};
    static uint8_t dist_index = 0;
    uint16_t current_dist = afstand_meten();
    
    distance_history[dist_index] = current_dist;
    dist_index = (dist_index + 1) % 4;
    
    uint32_t sum = 0;
    uint8_t count = 0;
    for (uint8_t i = 0; i < 4; i++) {
        if (distance_history[i] > 0) {
            sum += distance_history[i];
            count++;
        }
    }
    if (count == 0) return 0;
    return sum / count;
}

int main(void){
    DDRB |= (1 << TRIG_PIN);
    DDRB &= ~(1 << ECHO_PIN);
    
    adc_init();
    setup_timer1_for_distance();
    setup_timer2_for_sound();
    sei();
    
    while(1){
        trigger_sensor();
        _delay_ms(50);
        
        uint16_t afstand = smooth_afstand_meten();
        target_freq = bereken_frequentie(afstand);
        smooth_frequency_update();
        
        _delay_ms(50);
    }
}