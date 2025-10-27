#ifndef BEPAAL_TOONHOOGTE_H
#define BEPAAL_TOONHOOGTE_H

#include <stdint.h>

void setup_timer2_sound(void);
void setup_timer1_sensor(void);
void update_freq(uint16_t *freq);
void smooth_freq(uint16_t *current, uint16_t target);
uint16_t read_distance(void);
uint16_t dist_to_freq(uint16_t dist);

#endif