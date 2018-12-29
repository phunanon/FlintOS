#include "Display.hpp"
#include <util/delay.h>

#define USE_SSD1306_128x64
//#define USE_SSD1306_64x32
#include "ssd1306/ssd1306_64x32.c"

Display::Display ()
{
  _delay_ms(40);
  ssd1306_init();
  clearScreen();
}

void Display::print (char* str, bool carriage)
{
  char* s = str;
  ssd1306_send_data_start();
  do {
    pch(*s);
  } while (*(++s));
  ssd1306_send_data_stop();
  
  if (carriage) nl();
}

void Display::print (char c)
{
  if (c == 0x7F) nl();
  else {
    ssd1306_send_data_start();
    pch(c);
    ssd1306_send_data_stop();
  }
}

void Display::printColumn (uint8_t b)
{
  ssd1306_send_data_start();
  ssd1306_send_byte(b);
  ssd1306_send_data_stop();
  ++col;
  if (col >= USEABLE_WIDTH) nl();
}

void Display::printHexU4 (uint8_t n)
{
  char c = (n < 10) ? (c = '0' + n) : ('A' + (n-10));
  print(c);
}

void Display::printHexU8 (uint8_t b)
{
  printHexU4((b & 0xF0) >> 4);
  printHexU4(b & 0x0F);
}

void Display::printHexU16 (uint16_t w)
{
  printHexU8((w & 0xFF00) >> 8);
  printHexU8(w & 0xFF);
}

void Display::clearScreen ()
{
  ssd1306_fill(0x00);
  col = 0;
  page = 0;
}

void Display::clearLine ()
{
  ssd1306_fill_page(page, 0x00);
  col = 0;
}

void Display::pch (char ch)
{
  ssd1306_char(ch);
  col += CHAR_WIDTH;
  if (col >= USEABLE_WIDTH) {
    ssd1306_send_data_stop();
    nl();
    ssd1306_send_data_start();
  }
}

void Display::nl ()
{
  if (page == PAGES - 1) page = -1;
  ssd1306_setpos(0, ++page);
  clearLine();
}
