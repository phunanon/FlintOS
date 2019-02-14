#include "Knapper.hpp"
#include <EEPROM.h>
#include "Arduino.h"

uint8_t Knapper::NextByte () { return EEPROM.read(_o++); }
uint16_t Knapper::NextWord () { return NextByte() | (NextByte() << 8); }

void Knapper::Set16Bit (uint16_t n)
{
  *(_pMem+0) =  n & 0x00FF;
  *(_pMem+1) = (n & 0xFF00) >> 8;
}

void Knapper::Set32Bit (uint32_t n)
{
  *(_pMem+0) = (n & 0x000000FF);
  *(_pMem+1) = (n & 0x0000FF00) >> 8;
  *(_pMem+2) = (n & 0x00FF0000) >> 16;
  *(_pMem+3) = (n & 0xFF000000) >> 24;
}

uint16_t Knapper::Get16Bit (uint8_t* p) { return uint16_t(*p) | uint16_t(*(p+1) << 8); }

uint32_t Knapper::Get32Bit (uint8_t* p)
{
  uint32_t n;
  n  = uint32_t(*(p+0));
  n |= uint32_t(*(p+1)) << 8;
  n |= uint32_t(*(p+2)) << 16;
  n |= uint32_t(*(p+3)) << 24;
  return n;
}

void Knapper::CV (uint8_t var)
{
  uint8_t i1;
  int16_t i2;
  int32_t i4;
  bool do2 = false, do4 = false;
  
  switch (var) {
    case 0x00: i1 = _typ->GetUserByte();        break;
    case 0x01: i1 = _typ->GetUserByte(_dis);    break;
    case 0x02: i1 = _typ->GetUserNibble();      break;
    case 0x03: i1 = _typ->GetUserNibble(_dis);  break;
    case 0x04: i1 = _typ->GetUserBit();         break;
    case 0x05: i1 = _typ->GetUserBit(_dis);     break;
    case 0x10: do4 = i4 = (int32_t)millis();    break;
    case 0x20: do2 = i2 = _entry;               break;
    default: i1 = i2 = i4 = 0;                  break;
  }
  
  if (do4) Set32Bit(i4);
  else if (do2) Set16Bit(i2);
  else *_pMem = i1;
}

void Knapper::J (uint8_t j) { _o = _entry + j; }

void Knapper::ExecuteEEPROM (uint16_t entry, Display* dis, Typer* typ)
{
  _o = _entry = entry;
  _dis = dis;
  _typ = typ;
  bool noHalt = true;
  while (noHalt) {
    uint8_t op = NextByte();
    if (op >= 0x02 && op <= 0x06) {                                 //JU-JG
      uint8_t j = NextByte();
      bool condition = false;
      if      (op == 0x02) condition = true;                        //JU
      else if (op == 0x03) condition = *_pMem == 0;                 //JZ
      else if (op == 0x04) condition = _mem[NextByte()] == *_pMem;  //JE
      else if (op == 0x05) condition = _mem[NextByte()] > *_pMem;   //JG
      else if (op == 0x06)                                          //J4
        condition = Get32Bit(_mem + NextByte()) == Get32Bit(_pMem);
      if (condition) J(j);
    } else if (op == 0x14 || op == 0x15) {                          //VL/LV
      uint8_t x = 0, len = NextByte(), start = NextByte();
      for (; x < len; ++x)
        if (op == 0x14) *(_pMem + x) = _mem[start + x];
        else _mem[start + x] = *(_pMem + x);
    } else if (op >= 0x20 && op <= 0x2B) {                          //1A-4R
      uint8_t* x = _mem + NextByte();
      uint8_t num1 = *x;
      if      (op == 0x20) *_pMem += num1;  //1A
      else if (op == 0x21) *_pMem -= num1;  //1S
      else if (op == 0x22) *_pMem *= num1;  //1M
      else if (op == 0x23) *_pMem /= num1;  //1D
      else if (op == 0x24) *_pMem %= num1;  //1L
      else if (op >= 0x25 && op <= 0x29) {
        uint32_t n = Get32Bit(_pMem);
        uint32_t num4 = Get32Bit(x);
        if      (op == 0x25) n += num4;     //4A
        else if (op == 0x26) n -= num4;     //4S
        else if (op == 0x27) n *= num4;     //4M
        else if (op == 0x28) n /= num4;     //4D
        else if (op == 0x29) n %= num4;     //4L
        Set32Bit(n);
      }
      else if (op == 0x2A)                  //1R
        *_pMem = (*_pMem >> num1) | (*_pMem << (8 - num1));
      else if (op == 0x2B) {                //4R
        uint32_t n = Get32Bit(_pMem);
        Set32Bit((n >> num1) | (n << (32 - num1)));
      }
    } else if ((op >= 0x43 && op <= 0x45) || op == 0x64) {  //CL/XL/BL/EL
      uint8_t* len = _pMem + _mem[NextByte()];
      for (uint8_t* x = _pMem; x < len; ++x) {
        if      (op == 0x43) _dis->print(*x);
        else if (op == 0x44) _dis->printHexU8(*x);
        else if (op == 0x45) _dis->printColumn(*x);
        else if (op == 0x64) *x = EEPROM.read(_ePtr++);
      }
    } else {
      switch (op) {
        case 0x00: J(*_pMem);                           break; //J_
        case 0x01: _o = NextWord();                     break; //JA
        case 0x0A: ++_pMem;                             break; //MI
        case 0x0B: --_pMem;                             break; //MD
        case 0x0C: _pMem += NextByte();                 break; //MA
        case 0x0D: _pMem -= NextByte();                 break; //MS
        case 0x10: *_pMem = NextByte();                 break; //S_
        case 0x11: _mem[NextByte()] = _mem[NextByte()]; break; //CP
        case 0x12: *_pMem = _mem[NextByte()];           break; //V_
        case 0x13: _mem[NextByte()] = *_pMem;           break; //_V
        case 0x2C: ++*_pMem;                            break; //1I
        case 0x2D: --*_pMem;                            break; //1D
        case 0x2E: Set32Bit(Get32Bit(_pMem) + 1);       break; //4I
        case 0x2F: Set32Bit(Get32Bit(_pMem) - 1);       break; //4D
        case 0x30: CV(NextByte());                      break; //CV
        case 0x40: _dis->print(*_pMem);                 break; //PC
        case 0x41: _dis->printHexU8(*_pMem);            break; //PX
        case 0x42: _dis->printColumn(*_pMem);           break; //PB
        case 0x60: _ePtr = Get16Bit(_pMem);             break; //ES
        case 0x61: Set16Bit(_ePtr);                     break; //EG
        case 0x62: *_pMem = EEPROM.read(_ePtr++);       break; //ER
        case 0x63: EEPROM.update(_ePtr++, *_pMem);      break; //EW
        case 0xFF: noHalt = false;                      break; //HT
      }
    }
  }
}
