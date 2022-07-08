#pragma once

#include "mcu/mcu.h"

namespace mcu {

class Timer {
    public:
        static void begin();
        static uint32_t millis();
        static uint32_t uptime_sec();
        static bool elapsed_one_sec();
};

}
