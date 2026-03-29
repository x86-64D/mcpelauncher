#pragma once

#include "chunk_header.h"
#include <string>
#include <stdexcept>

namespace axml {

struct StringPoolHeader : public ChunkHeader {
    enum {
        UTF8_FLAG = 1 << 8
    };

    euint32 stringCount;
    euint32 styleCount;
    euint32 flags;
    euint32 stringsStart;
    euint32 stylesStart;
};

class StringPool {

private:
    StringPoolHeader* header;
    bool utf8;

    void assertHasHeader() const {
        if (header == nullptr)
            throw std::runtime_error("No header set");
    }

    static void assertSize(bool cond) {
        if (!cond)
            throw std::out_of_range("Bad size");
    }

public:
    StringPool() : header(nullptr) {}
    StringPool(StringPoolHeader* header) : header(header) {
        if (header == nullptr)
            return;
        utf8 = (header->flags & StringPoolHeader::UTF8_FLAG) != 0;
    }

    static size_t decodeLength(const uint8_t* t, size_t len, size_t& size);

    static size_t decodeLength(const uint16_t* t, size_t len, size_t& size);

    std::string getStringAtOffset(size_t offset) const;

    std::string getString(size_t index) const;

    size_t getStringCount() const { return header->stringCount; }


};


}