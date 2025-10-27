#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "bepaal_volume.h"

int main(void){
    adc_init();
    timer2_init();
    
    while(1){
        OCR2B = volume;                       // change loudness
    }
}