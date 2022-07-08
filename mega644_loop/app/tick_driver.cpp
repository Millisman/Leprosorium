#include "tick_driver.h"

extern stream::UartStream cout;

namespace app {

TickDriver::TickDriver()
{
}

void TickDriver::onEvent(const event::Event& event) {
    if (event.id == events::UPDATE) {
        // idle code
    } else if (event.id == events::TICK_1_SEC) {
        cout << "TICK_1_SEC, ms=" << mcu::Timer::millis() << " UPTIME, s=" << mcu::Timer::uptime_sec() << '\n';

    } else if (event.id == events::TICK_10_SEC) {
        cout << "TICK_10_SEC, ms=" << mcu::Timer::millis() << '\n';
    }
}

}
