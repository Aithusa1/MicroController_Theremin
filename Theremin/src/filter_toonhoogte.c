#include <avr/io.h>
#include <stdlib.h>
#include "filter_toonhoogte.h"

// Filter buffer volgens technisch ontwerp
uint8_t filter_size = 5;  // Start met medium filter grootte
FilterElement filter_buffer[FILTER_MAX_SIZE];

// Compare functie voor qsort (sorteren op value)
int compare_filter_elements(const void *a, const void *b) {
    const FilterElement *elemA = (const FilterElement *)a;
    const FilterElement *elemB = (const FilterElement *)b;
    
    if (elemA->value < elemB->value) return -1;
    if (elemA->value > elemB->value) return 1;
    return 0;
}

// Filter initialisatie
void filter_init(void) {
    for (uint8_t i = 0; i < FILTER_MAX_SIZE; i++) {
        filter_buffer[i].value = 800;  // Start op midden frequentie
        filter_buffer[i].age = i;      // Oudste eerst
    }
}

// Waarde toevoegen aan filter volgens technisch ontwerp algoritme
void filter_add_value(uint16_t value) {
    // Verhoog leeftijd van alle elementen
    for (uint8_t i = 0; i < filter_size; i++) {
        if (filter_buffer[i].age < 255) {
            filter_buffer[i].age++;
        }
    }
    
    // Zoek oudste element
    uint8_t oldest_index = 0;
    uint8_t oldest_age = filter_buffer[0].age;
    
    for (uint8_t i = 1; i < filter_size; i++) {
        if (filter_buffer[i].age > oldest_age) {
            oldest_age = filter_buffer[i].age;
            oldest_index = i;
        }
    }
    
    // Vervang oudste element met nieuwe waarde
    filter_buffer[oldest_index].value = value;
    filter_buffer[oldest_index].age = 0;
}

// Mediaan bepalen volgens technisch ontwerp
uint16_t filter_get_median(void) {
    if (filter_size == 1) {
        return filter_buffer[0].value;  // Geen filtering als size = 1
    }
    
    // Maak tijdelijke kopie voor sortering
    FilterElement temp_buffer[FILTER_MAX_SIZE];
    for (uint8_t i = 0; i < filter_size; i++) {
        temp_buffer[i] = filter_buffer[i];
    }
    
    // Sorteer op waarde (quicksort volgens technisch ontwerp)
    qsort(temp_buffer, filter_size, sizeof(FilterElement), compare_filter_elements);
    
    // Bepaal mediaan (middelste element)
    // Volgens technisch ontwerp: (arraylengte / 2) + 1
    uint8_t median_index = (filter_size / 2);
    
    return temp_buffer[median_index].value;
}

// Filter grootte instellen (alleen oneven getallen volgens functioneel ontwerp)
void filter_set_size(uint8_t new_size) {
    if (new_size < FILTER_MIN_SIZE) new_size = FILTER_MIN_SIZE;
    if (new_size > FILTER_MAX_SIZE) new_size = FILTER_MAX_SIZE;
    
    // Zorg voor oneven getallen volgens UC.6
    if (new_size % 2 == 0) {
        new_size--;  // Maak oneven
        if (new_size < FILTER_MIN_SIZE) new_size = FILTER_MIN_SIZE;
    }
    
    filter_size = new_size;
    
    // Herinitialiseer filter bij grootte wijziging
    filter_init();
}

// Huidige filter grootte opvragen
uint8_t filter_get_size(void) {
    return filter_size;
}

// Pointer naar filter_size teruggeven
uint8_t* filter_get_size_ptr(void) {
    return &filter_size;
}