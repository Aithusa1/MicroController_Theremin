#ifndef FILTER_TOONHOOGTE_H
#define FILTER_TOONHOOGTE_H

#include <stdint.h>

#define FILTER_MIN_SIZE 1
#define FILTER_MAX_SIZE 15

// Struct voor filter elementen volgens technisch ontwerp
typedef struct {
    uint16_t value;  // De gemeten waarde
    uint8_t age;     // Leeftijd van de meting (0 = nieuwste)
} FilterElement;

// Externe variabelen
extern volatile uint8_t filter_size;
extern FilterElement filter_buffer[FILTER_MAX_SIZE];

// Functie declaraties
void filter_init(void);
void filter_add_value(uint16_t value);
uint16_t filter_get_median(void);
void filter_set_size(uint8_t new_size);
uint8_t filter_get_size(void);

#endif