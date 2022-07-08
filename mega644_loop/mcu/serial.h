#pragma once
#include "mcu/mcu.h"
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>

#include "utils/cpp.h"

namespace mcu {

class SerOut {
public:
    static void begin(uint16_t baud_setting);
    static void write(const uint8_t);
    static void write(const char *str) { while (*str) write(*str++); }
};

}
