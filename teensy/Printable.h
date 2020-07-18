//
// Created by Nicholas Newdigate on 10/03/2018.
//

#ifndef ARDUINO_ABSTRACTION_PRINTABLE_H
#define ARDUINO_ABSTRACTION_PRINTABLE_H

#include "Print.h"

class Print;

class Printable
{
public:
    virtual size_t printTo(Print& p) const = 0;
};


#endif //ARDUINO_ABSTRACTION_PRINTABLE_H
