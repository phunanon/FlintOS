#include "Typer.hpp"
#include <avr/io.h>
#include <util/delay.h>

#define DR(PIN) (PINB & (1 << PIN))
#define ANALOG_START() ADCSRA |= (1 << ADSC)
#define ANALOG_WAIT() while (ADCSRA & (1 << ADSC)) 

uint8_t Typer::GetBtn ()
{
  _delay_ms(200); //Mitigate double presses. TODO: use timer
  while (true) {
    uint8_t k = 0;
    if (KeyPressed(&k)) return k;
  }
}

bool Typer::KeyPressed (uint8_t* k)
{
  ANALOG_START();
  ANALOG_WAIT();
  bool isPressed = ADCH < 0xF7;
  if (isPressed && k) *k = (ADCH + 5) >> 4;
  return isPressed;
}

bool Typer::GetUserBit (Display* dis)
{
  bool b = !!GetBtn();
  if (dis) dis->printHexU4(b);
  return b;
}

uint8_t Typer::GetUserNibble (Display* dis)
{
  uint8_t n = GetBtn();
  if (dis) dis->printHexU4(n);
  return n;
}

uint8_t Typer::GetUserByte (Display* dis)
{
  uint8_t n1 = GetUserNibble(dis);
  uint8_t n2 = GetUserNibble(dis);
  return (n1 << 4) | n2;
}
