#pragma once

#include "chunk_header.h"

namespace axml {

struct ResValue {
    enum {
        TYPE_NULL = 0,

        TYPE_INT_FIRST = 0x10,
        TYPE_INT_DEC = 0x10,
        TYPE_INT_LAST = 0x1f
    };

    euint16 size;
    euint8 res0;
    euint8 dataType;
    euint32 data;

    bool isInteger() const {
        return dataType >= TYPE_INT_FIRST && dataType <= TYPE_INT_LAST;
    }
};

struct XMLNodeChunkHeader : public ChunkHeader {
    euint32 lineNumber;
    StringRef comment;
};


struct NamespaceChunkData {
    StringRef prefix;
    StringRef url;
};

struct ElementChunkData {
    StringRef ns;
    StringRef name;
};
struct StartElementChunkData : public ElementChunkData {
    euint16 attributeStart;
    euint16 attributeSize;
    euint16 attributeCount;
    euint16 idIndex;
    euint16 idClass;
    euint16 idStyle;
};

struct AttributeChunkData {
    StringRef ns;
    StringRef name;
    StringRef rawValue;
    ResValue typedValue;
};

}