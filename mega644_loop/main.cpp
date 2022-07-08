#include "mcu/watchdog.h"
#include "mcu/serial.h"
#include "stream/uartstream.h"
#include "event/loop.h"
#include "events.h"
#include "app/tick_driver.h"

stream::UartStream cout;


int main() {
    mcu::Watchdog::enable();
    mcu::Timer::begin();
    mcu::SerOut::begin((F_CPU / 4 / 115200 - 1) / 2);

    app::TickDriver::init(); // call constructor on init

    while (1) {
        if (mcu::Timer::elapsed_one_sec()) {
            event::Loop::post(event::Event(events::TICK_1_SEC));
        } else {
            event::Loop::post(event::Event(events::UPDATE));
        }
        event::Loop::dispatch();
        mcu::Watchdog::reset();
    }
}
