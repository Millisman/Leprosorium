#include "serial.h"

#if defined (__AVR__) || defined (__AVR_ARCH__)

#else

#include <iostream>
#include <string>

namespace mcu {

void SerOut::begin(uint16_t baud_setting) {
    std::cout << __FILE__ ":" << __LINE__ << ":" << __FUNCTION__
    << "(baud_setting = " << baud_setting << ")\n";
}
void SerOut::write(const uint8_t data) {
    std::cout << (char)data;
}


}
#endif



