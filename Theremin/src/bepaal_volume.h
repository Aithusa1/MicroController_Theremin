#ifndef BEPAAL_VOLUME_H
#define BEPAAL_VOLUME_H

#include <stdint.h>

extern volatile uint8_t volume;
extern volatile uint8_t sound_enabled;

void setup_adc(void);
void setup_timer2_volume_pwm(void);

#endif