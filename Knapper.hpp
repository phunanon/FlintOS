#pragma once

#include <stdint.h>
#include "Display.hpp"
#include "Typer.hpp"

class Knapper
{
private:
  uint8_t _mem[256] = {0};
  uint8_t* _pMem = _mem;
  uint16_t _entry = 0;
  uint16_t _o = 0;
  uint16_t _ePtr = 0;
  Display* _dis;
  Typer* _typ;
  
  uint8_t NextByte ();
  uint16_t NextWord ();
  uint16_t Get16Bit (uint8_t* p);
  uint32_t Get32Bit (uint8_t* p);
  void Set16Bit (uint16_t n);
  void Set32Bit (uint32_t n);
  void CV (uint8_t var);
  void J (uint8_t j);
  
public:
  void ExecuteEEPROM (uint16_t entry, Display* dis, Typer* typ);
};
