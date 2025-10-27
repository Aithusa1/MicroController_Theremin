#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "twi.h"

#define LCD_ADDR 0x27 // adress

volatile uint8_t volume = 128;
volatile uint16_t target_freq = 200;
volatile uint16_t current_freq = 200;
volatile uint8_t sound_enabled = 1;

volatile uint16_t echo_time = 0;
volatile uint8_t measuring = 0;

#define TRIG_PIN PB1
#define ECHO_PIN PB0
#define BUZZER_PIN PD3

void setup_timer2_sound(void) {
    DDRD |= (1 << BUZZER_PIN);
    TCCR2A = (1 << WGM21);
    TCCR2B = (1 << CS22) | (1 << CS20);
    OCR2A = (F_CPU / (128 * 500)) - 1;
    TIMSK2 = (1 << OCIE2A);
}

ISR(TIMER2_COMPA_vect) {
    if (sound_enabled && volume > 10) {
        PORTD ^= (1 << BUZZER_PIN);
    } else {
        PORTD &= ~(1 << BUZZER_PIN);
    }
}

// Simpele LCD functies
void lcd_command(uint8_t cmd) {
    uint8_t high = (cmd & 0xF0) | 0x08;
    uint8_t low = ((cmd << 4) & 0xF0) | 0x08;
    
    TWI_MT_Start();
    TWI_Transmit_SLAW(LCD_ADDR);
    TWI_Transmit_Byte(high | 0x04);
    TWI_Transmit_Byte(high);
    TWI_Transmit_Byte(low | 0x04);
    TWI_Transmit_Byte(low);
    TWI_Stop();
    _delay_us(100);
}

void lcd_data(uint8_t data) {
    uint8_t high = (data & 0xF0) | 0x09;
    uint8_t low = ((data << 4) & 0xF0) | 0x09;
    
    TWI_MT_Start();
    TWI_Transmit_SLAW(LCD_ADDR);
    TWI_Transmit_Byte(high | 0x04);
    TWI_Transmit_Byte(high);
    TWI_Transmit_Byte(low | 0x04);
    TWI_Transmit_Byte(low);
    TWI_Stop();
    _delay_us(100);
}

void lcd_print(char *str) {
    while(*str) {
        lcd_data(*str++);
    }

}

void lcd_init() {
    _delay_ms(50);
    
    // Initialisatie
    lcd_command(0x33);
    _delay_ms(5);
    lcd_command(0x32);
    _delay_ms(5);
    lcd_command(0x28); // 2 lines, 5x8 font
    lcd_command(0x0C); // Display on, cursor off
    lcd_command(0x06); // Entry mode
    lcd_command(0x01); // Clear display
    _delay_ms(2);
}


// Update de frequentie van de toon
void update_freq(uint16_t *freq) {
    if (*freq < 50) *freq = 50;
    if (*freq > 2000) *freq = 2000;
    uint32_t ocr = (F_CPU / (128UL * (*freq))) - 1;
    if (ocr > 255) ocr = 255;
    OCR2A = ocr;
}

void smooth_freq(uint16_t *current, uint16_t target) {
    int16_t diff = target - *current;
    if (diff > 0) *current += diff / 8;
    else if (diff < 0) *current += diff / 8;
    update_freq(current);
}

void setup_adc(void) {
    ADMUX = (1 << REFS0) | (1 << ADLAR);
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

ISR(ADC_vect) {
    volume = ADCH;
    sound_enabled = (volume > 10);
}

void setup_timer1_sensor(void) {
    TCCR1A = 0;
    TCCR1B = (1 << ICES1) | (1 << CS11);
    TIMSK1 = (1 << ICIE1);
}

ISR(TIMER1_CAPT_vect) {
    if (!measuring) {
        TCNT1 = 0;
        TCCR1B &= ~(1 << ICES1);
        measuring = 1;
    } else {
        echo_time = ICR1;
        TCCR1B |= (1 << ICES1);
        measuring = 0;
    }
}

uint16_t read_distance(void) {
    PORTB |= (1 << TRIG_PIN);
    _delay_us(10);
    PORTB &= ~(1 << TRIG_PIN);
    _delay_ms(50);
    
    if (!measuring && echo_time > 0) {
        uint16_t dist = (echo_time * 2) / 58;
        echo_time = 0;
        return (dist < 2 || dist > 400) ? 0 : dist;
    }
    return 0;
}

uint16_t dist_to_freq(uint16_t dist) {
    if (dist < 2 || dist > 65) return 150;
    return 2000 - ((dist - 2) * 1900) / 63;
}

int main(void) {
    TWI_Init();
    _delay_ms(1000);

    DDRB |= (1 << TRIG_PIN);
    DDRB &= ~(1 << ECHO_PIN);
    
    setup_adc();
    setup_timer1_sensor();
    setup_timer2_sound();
    sei();
    
    lcd_init();
    
    // Print "Hallo"
    lcd_print("Hallo");
    
    while(1) {
        _delay_ms(25);
        uint16_t dist = read_distance();
        target_freq = dist_to_freq(dist);
        smooth_freq(&current_freq, target_freq);
        _delay_ms(25);
    }
}



