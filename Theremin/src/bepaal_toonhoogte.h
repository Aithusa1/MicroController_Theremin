#ifndef BEPAAL_TOONHOOGTE_H
#define BEPAAL_TOONHOOGTE_H

#include <stdint.h>

uint16_t dist_to_freq(uint16_t distance);
void smooth_freq(volatile uint16_t *current, uint16_t target);

#endif