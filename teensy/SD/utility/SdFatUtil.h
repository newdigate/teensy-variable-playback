/* Arduino SdFat Library
 * Copyright (C) 2008 by William Greiman
 *
 * This file is part of the Arduino SdFat Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with the Arduino SdFat Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef SdFatUtil_h
#define SdFatUtil_h
/**
 * \file
 * Useful utility functions.
 */
#include "../../mock_arduino.h"

#define NOINLINE __attribute__((noinline,unused))
#define UNUSEDOK __attribute__((unused))
//------------------------------------------------------------------------------
/** Return the number of bytes currently free in RAM. */

#ifdef __AVR__
//------------------------------------------------------------------------------
/**
 * %Print a string in flash memory to the serial port.
 *
 * \param[in] str Pointer to string stored in flash memory.
 */
static NOINLINE void SerialPrint_P(PGM_P str) {
  for (uint8_t c; (c = pgm_read_byte(str)); str++) Serial.write(c);
}
//------------------------------------------------------------------------------
/**
 * %Print a string in flash memory followed by a CR/LF.
 *
 * \param[in] str Pointer to string stored in flash memory.
 */
static NOINLINE void SerialPrintln_P(PGM_P str) {
  SerialPrint_P(str);
  Serial.println();
}
#endif  // __AVR__
#endif  // #define SdFatUtil_h
