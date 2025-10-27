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

// Externe functies uit bepaal_toonhoogte.c
void setup_timer2_sound(void);
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