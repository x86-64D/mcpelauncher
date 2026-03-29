#pragma once

#include <cstddef>
#include "endian_aware_int.h"

namespace axml {

using StringRef = euint32;

struct ChunkHeader {
    enum {
        TYPE_STRING_POOL = 1,
        TYPE_XML = 3,

        TYPE_XML_START_NAMESPACE = 0x100,
        TYPE_XML_END_NAMESPACE = 0x101,
        TYPE_XML_START_ELEMENT = 0x102,
        TYPE_XML_END_ELEMENT = 0x103,
        TYPE_XML_CDATA = 0x104
    };

    euint16 type;
    euint16 headerSize;
    euint32 size;
};

}