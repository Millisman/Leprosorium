#pragma once

#include "event/handler.h"
#include "utils/cpp.h"
#include "events.h"
#include "event/loop.h"
#include "utils/crc.h"
#include "mcu/timer.h"
#include "mcu/serial.h"
#include "stream/uartstream.h"

namespace app {

class TickDriver:  public event::Handler {

public:
    TickDriver();

    static TickDriver& init() {
        static TickDriver mTime;
        return mTime;
    }

private:
    void onEvent(const event::Event& event) override;
    DISALLOW_COPY_AND_ASSIGN(TickDriver);
};

}
