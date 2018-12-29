#define F_CPU 1000000UL      //ATtiny85 CPU Clock speed (8MHz optimal[8000000], 1MHz Default[1000000])

#include <stdlib.h>
#include <util/delay.h>

#include "Display.hpp"
#include "Typer.hpp"
#include "Knapper.hpp"

Display dis = Display();
Typer typ = Typer();

void Execute (uint16_t entry)
{
  Knapper k = Knapper();
  k.ExecuteEEPROM(entry, &dis, &typ);
}

void setup ()
{
  ADCSRA = 0b10000011;  //enable ADC, single conversion mode,  division factor 8 for a 125kHz clock
  ADMUX = (1 << ADLAR)  // left shift result (8-bit mode)
    | (0 << REFS1)      // ref. voltage to VCC, bit 1
    | (0 << REFS0)      // ref. voltage to VCC, bit 0
    | (0 << MUX3)       // use ADC1 for input (PB4), MUX bit 3
    | (0 << MUX2)       //                         , MUX bit 2
    | (0 << MUX1)       //                         , MUX bit 1
    | (1 << MUX0);      //                         , MUX bit 0

  /*while (1) {
    ANALOG_START();
    ANALOG_WAIT();
    typ.GetUserNibble(&dis);
  }*/
  
  dis.print("       FlintOS", true);
  dis.print("any btn or boot 000");
  _delay_ms(2000);
  
  if (!typ.KeyPressed()) {
    dis.clearScreen();
    Execute(0x00);
  } else {
    dis.print("!", true);
    _delay_ms(1000);
  }
}

void loop()
{
  dis.print('>');
  uint16_t entry = 0;
  entry = typ.GetUserBit(&dis) << 8;
  entry |= typ.GetUserByte(&dis);
  dis.nl();
  Execute(entry);
  dis.nl();
}
