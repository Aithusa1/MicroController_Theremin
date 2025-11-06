#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "bepaal_toonhoogte.h"
#include "bepaal_volume.h"

// Externe variabelen gedefinieerd in bepaal_volume.c
extern volatile uint8_t volume;
extern volatile uint8_t sound_enabled;

// Globale variabelen voor afstandsmeting en buzzer
volatile uint16_t echo_time = 0;      // Tijd gemeten door input capture
volatile uint8_t measuring = 0;       // Status van meting (0=wacht op rising edge, 1=wacht op falling edge)
volatile uint8_t buzzer_enabled = 0;  // Staat van buzzer aan/uit voor frequentie generatie

// Timer0 initialisatie voor frequentie generatie (toonhoogte)
void setup_timer0_frequency(void) {
    // CTC mode: Timer telt tot OCR0A en reset dan
    TCCR0A = (1 << WGM01);
    // Prescaler 256 voor langzamere teller
    TCCR0B = (1 << CS02);
    // Compare match A interrupt inschakelen
    TIMSK0 = (1 << OCIE0A);
    // Startwaarde voor 800Hz: F_CPU / (prescaler * frequentie * 2)
    // *2 omdat we 2 interrupts per periode nodig hebben (aan/uit)
    OCR0A = (F_CPU / (256 * 800 * 2)) - 1;
}

// Timer0 compare match interrupt - genereert blokgolf voor toonhoogte
ISR(TIMER0_COMPA_vect) {
    // Toggle buzzer state voor blokgolf
    buzzer_enabled = !buzzer_enabled;
    
    // PWM aan/uit zetten op basis van buzzer state en volume
    if (sound_enabled && volume > 10 && buzzer_enabled) {
        TCCR2A |= (1 << COM2B1);  // PWM output aan
    } else {
        TCCR2A &= ~(1 << COM2B1); // PWM output uit
    }
}

// Frequentie bijwerken naar nieuwe waarde
void update_freq(uint16_t freq) {
    // Begrens frequentie volgens specificatie: 230Hz - 1400Hz
    if (freq < 230) freq = 230;
    if (freq > 1400) freq = 1400;
    
    // Bereken OCR waarde voor gewenste frequentie
    // Formule: f = F_CPU / (2 * N * (1 + OCR0A))
    // Omgekeerd: OCR0A = (F_CPU / (2 * N * f)) - 1
    uint32_t ocr = (16000000UL / (2UL * 256UL * freq)) - 1;
    
    // Begrens OCR waarde tot 8-bit bereik
    if (ocr > 255) ocr = 255;
    if (ocr < 1) ocr = 1;
    
    // Stel nieuwe compare waarde in
    OCR0A = ocr;
}

// Geleidelijke frequentie overgang voor vloeiende toonverandering
void smooth_freq(volatile uint16_t *current, uint16_t target) {
    // Bereken verschil tussen huidige en doel frequentie
    int16_t diff = target - *current;
    
    // Snelle aanpassing voor grote verschillen
    if (diff > 30) {
        *current += 30;  // Grote stap omhoog
    } else if (diff > 10) {
        *current += 10;  // Medium stap omhoog
    } else if (diff > 0) {
        *current += 1;   // Kleine stap omhoog
    } else if (diff < -30) {
        *current -= 30;  // Grote stap omlaag
    } else if (diff < -10) {
        *current -= 10;  // Medium stap omlaag
    } else if (diff < 0) {
        *current -= 1;   // Kleine stap omlaag
    }
    
    // Pas frequentie aan met nieuwe waarde
    update_freq(*current);
}

// Timer1 initialisatie voor input capture (afstandsmeting)
void setup_timer1_sensor(void) {
    TCCR1A = 0; // Normal mode
    // Input capture rising edge, prescaler 8
    TCCR1B = (1 << ICES1) | (1 << CS11);
    // Input capture interrupt inschakelen
    TIMSK1 = (1 << ICIE1);
}

// Timer1 input capture interrupt - meet pulsduur van echo signaal
ISR(TIMER1_CAPT_vect) {
    if (!measuring) {
        // Rising edge gedetecteerd - start meting
        TCNT1 = 0; // Reset timer
        TCCR1B &= ~(1 << ICES1); // Switch naar falling edge detection
        measuring = 1; // Markeer als bezig met meting
    } else {
        // Falling edge gedetecteerd - meting compleet
        echo_time = ICR1; // Lees gemeten tijd
        TCCR1B |= (1 << ICES1); // Switch terug naar rising edge detection
        measuring = 0; // Markeer meting als voltooid
    }
}

// Afstand meten met ultrasonic sensor
uint16_t read_distance(void) {
    // Stuur trigger puls
    PORTB |= (1 << TRIG_PIN);
    _delay_us(10); // 10μs puls
    PORTB &= ~(1 << TRIG_PIN);
    
    // Wacht op meting
    _delay_ms(50);
    
    // Als meting compleet is en tijd geldig
    if (!measuring && echo_time > 0) {
        // Bereken afstand: tijd (μs) / 58 = afstand (cm)
        uint16_t dist = (echo_time * 2) / 58;
        echo_time = 0; // Reset voor volgende meting
        // Filter ongeldige waarden (buiten sensor bereik)
        return (dist < 2 || dist > 400) ? 0 : dist;
    }
    return 0; // Geen geldige meting
}

// Converteer afstand naar frequentie volgens specificatie
uint16_t dist_to_freq(uint16_t dist) {
    // Geen signaal (afstand = 0) -> laagste frequentie
    if (dist == 0) return 230;
    // Dichtbij (≤2cm) -> hoogste frequentie
    if (dist < 2) return 1400;
    // Ver weg (≥65cm) -> laagste frequentie  
    if (dist > 65) return 230;
    
    // Lineaire mapping: 2cm->1400Hz, 65cm->230Hz
    // f = 1400 - ((dist - 2) * (1400 - 230)) / (65 - 2)
    return 1400 - ((dist - 2) * 1170) / 63;
}