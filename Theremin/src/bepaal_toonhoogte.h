#ifndef BEPAAL_TOONHOOGTE_H
#define BEPAAL_TOONHOOGTE_H

#include <stdint.h>

// Functie declaraties
uint16_t dist_to_freq(uint16_t dist);
void smooth_freq(uint16_t *current, uint16_t target);
void update_freq(uint16_t *freq);

#endif