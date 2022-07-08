#pragma once

#include "mcu/mcu.h"

namespace mcu {

class Watchdog {
    Watchdog();

public:
    static void enable();
    static void reset();
    static void forceRestart();
};

}

