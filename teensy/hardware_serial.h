/*
  HardwareSerial.h - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  Modified 28 September 2010 by Mark Sproul
  Modified 14 August 2012 by Alarus
  Modified 3 December 2013 by Matthijs Kooijman
*/

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <inttypes.h>
#include "Print.h"

class HardwareSerial {
public:
    void begin(unsigned long baud);
    void begin(unsigned long, uint8_t);
    void end();
    int available(void);
    int peek(void);
    int read(void);
    int availableForWrite(void);
    void flush(void);
    int write(uint8_t);
    inline int write(unsigned long n) { return write((uint8_t)n); }
    inline int write(long n) { return write((uint8_t)n); }
    inline int write(unsigned int n) { return write((uint8_t)n); }
    inline int write(int n) { return write((uint8_t)n); }
    int write(unsigned char const*, unsigned long);
    operator bool() { return true; }

    void print(char i);
    void print(char *i);
    void println();
    void printf(const char *s, ...);
};

extern HardwareSerial Serial;
#endif
