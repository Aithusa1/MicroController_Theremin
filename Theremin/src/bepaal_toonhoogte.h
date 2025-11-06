#ifndef BEPAAL_TOONHOOGTE_H
#define BEPAAL_TOONHOOGTE_H

#include <stdint.h>

// Pin definitions
#define TRIG_PIN PB1
#define ECHO_PIN PB0
#define BUZZER_PIN PD3

// Functie declaraties
void setup_timer0_frequency(void);
void setup_timer1_sensor(void);
void update_freq(uint16_t freq);
void smooth_freq(volatile uint16_t *current, uint16_t target);
uint16_t read_distance(void);
uint16_t dist_to_freq(uint16_t dist);

#endif