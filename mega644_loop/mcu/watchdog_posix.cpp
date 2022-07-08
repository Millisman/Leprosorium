#include "watchdog.h"

#if defined (__AVR__) || defined (__AVR_ARCH__)

#else

#include <iostream>
#include <string>

namespace mcu {

void Watchdog::enable() {
    std::cout << __FILE__ ":" << __LINE__ << ":" << __FUNCTION__ << "()\n";
}

void Watchdog::reset() {
    //std::cout << __FILE__ ":" << __LINE__ << ":" << __FUNCTION__ << "()\n";
}

void Watchdog::forceRestart() {
    std::cout << __FILE__ ":" << __LINE__ << ":" << __FUNCTION__ << "()\nGoodbye!\n";
    while (1) { }
}

}  // namespace mcu

#endif
