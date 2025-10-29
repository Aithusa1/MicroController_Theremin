#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "twi.h"
#include "weergeven_afstand_hoogte.h"
#include "bepaal_toonhoogte.h"
#include "bepaal_volume.h"
#include "filter_toonhoogte.h"

// Global variables - gedefinieerd in volume.c
extern volatile uint8_t volume;
extern volatile uint8_t sound_enabled;

volatile uint16_t target_freq = 800;  // Start in midden bereik
volatile uint16_t current_freq = 800;

void display_info(uint16_t dist, uint16_t freq) {
    lcd_command(0x80); // Eerste regel
    char buffer[16];
    
    if (dist > 0) {
        snprintf(buffer, sizeof(buffer), "Dist:%3dcm F:%2d", dist, filter_get_size());
    } else {
        snprintf(buffer, sizeof(buffer), "No signal   F:%2d", filter_get_size());
    }
    lcd_print(buffer);
    
    lcd_command(0xC0); // Tweede regel
    snprintf(buffer, sizeof(buffer), "F:%4dHz V:%3d", freq, volume);
    lcd_print(buffer);
}

void setup_filter_buttons(void) {
    // PD4 en PD5 als input met pull-up (volgens TO)
    DDRD &= ~((1 << PD4) | (1 << PD5));
    PORTD |= (1 << PD4) | (1 << PD5);
    
    // Pin Change Interrupt enable (TO: PCICR en PCMSK2)
    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << PCINT20) | (1 << PCINT21);
#include <util/delay.h>
#include "twi.h"
#include "weergeven_filter.h"

#define BUTTON_UP_PIN   PD4
#define BUTTON_DOWN_PIN PD5
#define BUTTON_UP_MASK  (1 << BUTTON_UP_PIN)
#define BUTTON_DOWN_MASK (1 << BUTTON_DOWN_PIN)

uint8_t filter_size = 1; // Start met filter grootte 1

void init_buttons(void) {
    // PD4 en PD5 als input met interne pull-up
    DDRD &= ~(BUTTON_UP_MASK | BUTTON_DOWN_MASK);
    PORTD |= (BUTTON_UP_MASK | BUTTON_DOWN_MASK);
}

uint8_t read_buttons(void) {
    uint8_t button_state = 0;
    
    // Lees knop status (actief laag vanwege pull-up)
    if (!(PIND & BUTTON_UP_MASK)) {
        button_state |= 0x01; // Knop omhoog ingedrukt
    }
    if (!(PIND & BUTTON_DOWN_MASK)) {
        button_state |= 0x02; // Knop omlaag ingedrukt
    }
    
    return button_state;
}

void update_filter_size(uint8_t buttons) {
    static uint8_t last_buttons = 0;
    
    // Detect rising edge (knop losgelaten) voor debounce
    uint8_t pressed = (last_buttons & ~buttons);
    last_buttons = buttons;
    
    if (pressed & 0x01) { // Knop omhoog losgelaten
        if (filter_size < 15) {
            filter_size++;
        }
    }
    if (pressed & 0x02) { // Knop omlaag losgelaten
        if (filter_size > 1) {
            filter_size--;
        }
    }
}

int main(void) {
    // Initialisatie
    TWI_Init();
    init_buttons();
    _delay_ms(100);
    
    // Toon initiële filter grootte
    display_filter_size(filter_size);

    while (1) {
        // Lees knoppen
        uint8_t buttons = read_buttons();
        
        // Update filter grootte als nodig
        update_filter_size(buttons);
        
        // Toon huidige filter grootte
        display_filter_size(filter_size);
        
        _delay_ms(100); // Korte vertraging voor debounce
    }
// Pin Change Interrupt voor filterknoppen (UC.6: Aanpassen filter)
ISR(PCINT2_vect) {
    static uint8_t last_state = 0xFF;
    uint8_t current_state = PIND & ((1 << PD4) | (1 << PD5));
    
    // Debounce
    if(current_state != last_state) {
        _delay_ms(50);
        current_state = PIND & ((1 << PD4) | (1 << PD5));
        
        if((current_state & (1 << PD4)) == 0) { // PD4 ingedrukt - filter kleiner
            uint8_t current_size = filter_get_size();
            if(current_size > FILTER_MIN_SIZE) {
                filter_set_size(current_size - 2); // Stap van 2 voor oneven getallen
            }
        }
        
        if((current_state & (1 << PD5)) == 0) { // PD5 ingedrukt - filter groter
            uint8_t current_size = filter_get_size();
            if(current_size < FILTER_MAX_SIZE) {
                filter_set_size(current_size + 2); // Stap van 2 voor oneven getallen
            }
        }
        
        last_state = current_state;
    }
}

int main(void) {
    // Pin configuratie
    DDRB |= (1 << TRIG_PIN);
    DDRB &= ~(1 << ECHO_PIN);
    
    // Initialisatie
    TWI_Init();
    _delay_ms(1000);
    lcd_init();
    filter_init();  // UC.2: Filter toonhoogte initialisatie
    
    // Hardware initialisatie VOLGENS TECHNISCH ONTWERP:
    setup_adc();                    // Volume ADC (UC.3)
    setup_timer2_volume_pwm();      // Timer2: Fast PWM voor volume
    setup_timer0_frequency();       // Timer0: CTC mode voor frequentie
    setup_timer1_sensor();          // Timer1: Sensor meting
    setup_filter_buttons();         // Filter knoppen (UC.6)
    
    // Start bericht
    lcd_command(0x01);
    lcd_print("Theremin Ready");
    _delay_ms(1000);
    
    // Global interrupts inschakelen
    sei();
    
    // In de hoofdloop:
    while(1) {
        uint16_t dist = read_distance();
        target_freq = dist_to_freq(dist);
        
        // FILTER TOEPASSEN volgens use case UC.2
        filter_add_value(target_freq);
        uint16_t filtered_freq = filter_get_median();
        
        smooth_freq(&current_freq, filtered_freq);
        
        // Update PWM duty cycle op basis van volume
        OCR2B = volume;
        
        // Display info
        display_info(dist, current_freq);
        
        _delay_ms(15);  // Zeer korte delay voor snelle respons
    }
}