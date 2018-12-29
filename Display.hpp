#pragma once

#include <stdint.h>

class Display
{
private:
  uint8_t page = 0;
  uint8_t col = 0;

  void CheckBounds ();
  void pch (char ch);

public:
  Display ();
  void print (char* str, bool carriage = false);
  void print (char c);
  void printColumn (uint8_t b);
  void printHexU4 (uint8_t n);
  void printHexU8 (uint8_t b);
  void printHexU16 (uint16_t w);
  void clearScreen ();
  void clearLine ();
  void nl ();
};
