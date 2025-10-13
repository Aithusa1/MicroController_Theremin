#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

uint8_t freq = 50;
void setup_timer0(void)
{
  TCCR0A |= (1<<WGM01);
  TCCR0B |= (1<<CS02);
  OCR0A = 70;
  TCNT0 = 0;
  TIMSK0 |= (1<<OCIE0A);
  sei();
}

ISR(TIMER0_COMPA_vect)
{
  PORTD ^= (1<<PD3);
}

void afstand_meten()
{

}

void setup_timer0(void);

int main()
{
  int freq = 0;
  DDRD |= (1<<PD3);
  setup_timer0();

  while(1)
  {
  
  }
  return 0;
}