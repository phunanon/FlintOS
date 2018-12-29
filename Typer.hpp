#pragma once

#include <stdint.h>
#include "Display.hpp"

class Typer
{
private:
  uint8_t GetBtn ();

public:
  bool KeyPressed (uint8_t* k = nullptr);
  bool GetUserBit (Display* dis = nullptr);
  uint8_t GetUserNibble (Display* dis = nullptr);
  uint8_t GetUserByte (Display* dis = nullptr);
};
