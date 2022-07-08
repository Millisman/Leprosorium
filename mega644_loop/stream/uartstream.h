#pragma once

#include "mcu/serial.h"
#include "outputstream.h"

namespace stream {

class UartStream : public OutputStream {
private:
    void write(const char ch) override {
        if (ch == '\n') mcu::SerOut::write('\r');
        mcu::SerOut::write(ch);
    }
};

}
