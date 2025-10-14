#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "twi.h"

// ================= PIN DEFINITIES =================
#define TRIG_PIN PB1   // D9
#define ECHO_PIN PB0   // D8
#define LED_PIN PD6    // D6 (OC0A)
#define POT_PIN PC0    // A0

// ================= LCD I2C DEFINITIES =================
#define LCD_I2C_ADDR 0x27  // typ. 0x27 of 0x3F, afhankelijk van backpack
#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE    0x04
#define LCD_COMMAND   0
#define LCD_DATA      1

// ================= FUNCTIEPROTOTYPES =================
void timer0_pwm_init(void);
void adc_init(void);
uint16_t adc_read(void);
void ultrasonic_init(void);
float ultrasonic_read_cm(void);
void display_7seg_zero(void);
void lcd_init_display(void);

// ================= LCD I2C FUNCTIES =================
void lcd_i2c_write(uint8_t data)
{
    TWI_MT_Start();
    TWI_Transmit_SLAW(LCD_I2C_ADDR);
    TWI_Transmit_Byte(data);
    TWI_Stop();
    _delay_us(50);
}

void lcd_i2c_send(uint8_t value, uint8_t mode)
{
    uint8_t highnib = value & 0xF0;
    uint8_t lownib = (value << 4) & 0xF0;
    uint8_t data;

    // verzend high en low nibble (4-bit modus)
    for (int i = 0; i < 2; i++) {
        data = (i == 0 ? highnib : lownib);
        lcd_i2c_write(data | mode | LCD_BACKLIGHT | LCD_ENABLE);
        lcd_i2c_write(data | mode | LCD_BACKLIGHT);
    }
}

void lcd_init_i2c(void)
{
    TWI_Init();
    _delay_ms(50);

    lcd_i2c_send(0x33, LCD_COMMAND);
    lcd_i2c_send(0x32, LCD_COMMAND);
    lcd_i2c_send(0x28, LCD_COMMAND); // 4-bit, 2 lijnen
    lcd_i2c_send(0x0C, LCD_COMMAND); // display aan, cursor uit
    lcd_i2c_send(0x06, LCD_COMMAND); // auto increment
    lcd_i2c_send(0x01, LCD_COMMAND); // clear
    _delay_ms(5);
}

void lcd_clear_i2c(void)
{
    lcd_i2c_send(0x01, LCD_COMMAND);
    _delay_ms(2);
}

void lcd_set_cursor_i2c(uint8_t col, uint8_t row)
{
    uint8_t row_offsets[] = {0x00, 0x40};
    lcd_i2c_send(0x80 | (col + row_offsets[row]), LCD_COMMAND);
}

void lcd_print_i2c(const char *str)
{
    while (*str) {
        lcd_i2c_send(*str++, LCD_DATA);
    }
}

// ================= LCD INIT DISPLAY =================
void lcd_init_display(void)
{
    lcd_init_i2c();
    lcd_clear_i2c();
    lcd_set_cursor_i2c(0,0);
    lcd_print_i2c("Theremin Demo");
    _delay_ms(1000);
    lcd_clear_i2c();
}

// ================= ADC =================
void adc_init(void)
{
    ADMUX = (1<<REFS0); // AVCC referentie, kanaal ADC0
    ADCSRA = (1<<ADEN) | (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // enable ADC, prescaler 128
}

uint16_t adc_read(void)
{
    ADMUX = (ADMUX & 0xF0); // kanaal 0
    ADCSRA |= (1<<ADSC);
    while(ADCSRA & (1<<ADSC)); // wacht op einde conversie
    return ADC;
}

// ================= PWM LED =================
void timer0_pwm_init(void)
{
    DDRD |= (1<<LED_PIN); // D6 als output
    TCCR0A = (1<<COM0A1) | (1<<WGM01) | (1<<WGM00); // Fast PWM, non-inverting
    TCCR0B = (1<<CS01); // prescaler 8
}

// ================= ULTRASOON =================
void ultrasonic_init(void)
{
    DDRB |= (1<<TRIG_PIN);   // Trigger = output
    DDRB &= ~(1<<ECHO_PIN);  // Echo = input
}

float ultrasonic_read_cm(void)
{
    uint16_t count;
    float distance;

    // trigger pulse
    PORTB &= ~(1<<TRIG_PIN);
    _delay_us(2);
    PORTB |= (1<<TRIG_PIN);
    _delay_us(10);
    PORTB &= ~(1<<TRIG_PIN);

    // wacht op echo high
    while(!(PINB & (1<<ECHO_PIN)));
    TCNT1 = 0;
    TCCR1B = (1<<CS11); // prescaler 8, start timer

    // wacht tot echo low
    while(PINB & (1<<ECHO_PIN));
    TCCR1B = 0; // stop timer
    count = TCNT1;

    // afstand (tijd in µs / 58 = cm)
    distance = (float)count / 2.0 / 58.0;
    return distance;
}

// ================= 7-SEGMENT VIA I2C =================
void display_7seg_zero(void)
{
    // voorbeeld: stuur via I2C naar PCF8574 op adres 0x38
    TWI_MT_Start();
    TWI_Transmit_SLAW(0x38);
    TWI_Transmit_Byte(0x3F); // patroon voor '0'
    TWI_Stop();
}

// ================= MAIN =================
int main(void)
{
    float distance;
    uint16_t potvalue;
    char buffer[16];

    // subsystemen init
    lcd_init_display();
    adc_init();
    timer0_pwm_init();
    ultrasonic_init();
    TCCR1A = 0;
    TCCR1B = 0;
    display_7seg_zero();

    while(1)
    {
        // 1️⃣ Lees afstand en toon op LCD
        distance = ultrasonic_read_cm();
        lcd_clear_i2c();
        lcd_set_cursor_i2c(0,0);
        snprintf(buffer, 16, "Afstand: %.1fcm", distance);
        lcd_print_i2c(buffer);

        // 2️⃣ Lees potmeter en dim LED
        potvalue = adc_read();
        OCR0A = potvalue >> 2; // schaal 10-bit naar 8-bit PWM
        lcd_set_cursor_i2c(0,1);
        snprintf(buffer, 16, "Pot: %u", potvalue);
        lcd_print_i2c(buffer);

        _delay_ms(200);
    }
}
