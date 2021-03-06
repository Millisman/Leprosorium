#pragma once

#include <stdint.h>
#include "stream/outputstream.h"

namespace utils {

void Hexdump(stream::OutputStream& out, const uint8_t RowSize, bool ShowAscii, const void* mData, const uint8_t mLength);

}
