#include <avr/io.h>
#include <stdint.h>
#include "bepaal_toonhoogte.h"

#define F_CPU 16000000UL

// Zet afstand om naar frequentie
uint16_t dist_to_freq(uint16_t dist) {
    if (dist < 2 || dist > 65) return 150;
    return 2000 - ((dist - 2) * 1900) / 63;
}

// Update de frequentie van de toon
void update_freq(uint16_t *freq) {
    if (*freq < 50) *freq = 50;
    if (*freq > 2000) *freq = 2000;
    uint32_t ocr = (F_CPU / (128UL * (*freq))) - 1;
    if (ocr > 255) ocr = 255;
    OCR2A = ocr;
}

// Soepele overgang tussen frequenties
void smooth_freq(uint16_t *current, uint16_t target) {
    int16_t diff = target - *current;
    if (diff > 0) *current += diff / 8;
    else if (diff < 0) *current += diff / 8;
    update_freq(current);
}