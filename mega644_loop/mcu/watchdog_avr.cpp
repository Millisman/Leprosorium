#include "watchdog.h"

#if defined (__AVR__) || defined (__AVR_ARCH__)

#include <avr/wdt.h>

namespace mcu {

void Watchdog::enable() {
    wdt_enable(WDTO_4S);
}

void Watchdog::reset() {
    wdt_reset();
}

void Watchdog::forceRestart() {
    wdt_enable(WDTO_15MS);
    while (1) {}
}

}  // namespace mcu

#endif
