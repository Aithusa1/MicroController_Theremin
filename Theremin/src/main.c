
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "twi.h"
#include "bepaal_toonhoogte.h"
#include "weergeven_afstand_hoogte.h"
#include "bepaal_volume.h"
#include <stdio.h>

// Global variables
volatile uint16_t target_freq = 200;
volatile uint16_t current_freq = 200;
volatile uint8_t sound_enabled = 1;

volatile uint16_t echo_time = 0;
volatile uint8_t measuring = 0;

// Pin definitions
#define TRIG_PIN PB1
#define ECHO_PIN PB0
#define BUZZER_PIN PD3

// Timer2 voor geluidsgeneratie - CORRECTIE: Gebruik dezelfde instellingen als in bepaal_volume.c
void setup_timer2_sound(void) {
    DDRD |= (1 << BUZZER_PIN);
    // Fast PWM op OC2B, non-inverting (zoals in bepaal_volume.c)
    TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
    // Prescaler 64 voor PWM carrier (zoals in bepaal_volume.c)
    TCCR2B = (1 << CS22);
    OCR2A = 249; // Voor 1kHz PWM carrier (16000000/(64*1000) - 1)
    OCR2B = volume; // Start met huidig volume
}

// ADC voor volume regeling - CORRECTIE: Gebruik adc_init() uit bepaal_volume.c
void setup_adc(void) {
    adc_init(); // Gebruik de bestaande ADC initialisatie
}

extern void smooth_freq(volatile uint16_t *current, uint16_t target);

// Timer1 voor afstandssensor
void setup_timer1_sensor(void) {
    TCCR1A = 0;
    TCCR1B = (1 << ICES1) | (1 << CS11); // Noise canceler, prescaler 8
    TIMSK1 = (1 << ICIE1);
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

// Afstand uitlezen - CORRECTIE: Betere implementatie
uint16_t read_distance(void) {
    // Stuur trigger puls
    PORTB |= (1 << TRIG_PIN);
    _delay_us(10);
    PORTB &= ~(1 << TRIG_PIN);
    
    // Wacht op meting
    _delay_ms(50);
    
    if (!measuring && echo_time > 0) {
        // Bereken afstand in cm: (echo_time * 2) / 58
        // echo_time is in ticks met prescaler 8 (0.5µs per tick)
        // tijd in µs = echo_time * 0.5
        // afstand = (tijd * 0.034) / 2 = tijd * 0.017
        uint16_t dist_cm = (echo_time * 17) / 1000; // Vereenvoudigde berekening
        echo_time = 0;
        return (dist_cm < 2 || dist_cm > 400) ? 0 : dist_cm;
    }
    return 0;
}

int main(void) {
    // Initialisatie
    TWI_Init();
    _delay_ms(1000);

    // Pin configuratie
    DDRB |= (1 << TRIG_PIN);
    DDRB &= ~(1 << ECHO_PIN);
    PORTB &= ~(1 << TRIG_PIN);
    
    // Hardware initialisatie
    setup_adc();
    setup_timer1_sensor();
    setup_timer2_sound();
    
    // LCD initialisatie
    lcd_init();
    
    // Print startbericht
    lcd_command(0x01); // Clear display
    lcd_print("Theremin Ready");
    _delay_ms(1000);
    lcd_command(0x01); // Clear display
    
    // Global interrupts inschakelen
    sei();
    
    // Hoofdloop
    while(1) {
        uint16_t dist = read_distance();
        
        if (dist > 0) {
            // Toonhoogte berekenen
            target_freq = dist_to_freq(dist);
            
            // Toonhoogte smooth aanpassen
            smooth_freq(&current_freq, target_freq);
            
            // Volume update (PWM duty cycle)
            OCR2B = volume;
            
            // Display update
            lcd_command(0x80); // Eerste regel
            char buffer[16];
            // Toon afstand en frequentie
            snprintf(buffer, sizeof(buffer), "Dist:%3dcm", dist);
            lcd_print(buffer);
            
            lcd_command(0xC0); // Tweede regel
            snprintf(buffer, sizeof(buffer), "Freq:%4dHz", current_freq);
            lcd_print(buffer);
        } else {
            // Geen geldige meting
            sound_enabled = 0;
            lcd_command(0x80);
            lcd_print("No valid data");
            lcd_command(0xC0);
            lcd_print("                ");
        }
        
        _delay_ms(50);
    }
}