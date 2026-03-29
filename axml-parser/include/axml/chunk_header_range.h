#pragma once

#include "chunk_header.h"

namespace axml {

class ChunkHeaderRange {

private:
    ChunkHeader *pBegin, *pEnd;

public:
    class Iterator {
    private:
        ChunkHeader* header;
    public:
        Iterator(ChunkHeader* header) : header(header) {}

        ChunkHeader* operator ->() const { return header; }
        ChunkHeader* operator*() const { return header; }
        operator ChunkHeader*() const { return header; }

        Iterator& operator++() {
            header = (ChunkHeader*) ((size_t) header + header->size);
            return *this;
        }
    };

    ChunkHeaderRange() : pBegin(nullptr), pEnd(nullptr) {}

    ChunkHeaderRange(ChunkHeader* pBegin, ChunkHeader* pEnd) : pBegin(pBegin), pEnd(pEnd) {}

    ChunkHeaderRange(ChunkHeader* parent) : pBegin((ChunkHeader*) ((size_t) parent + parent->headerSize)),
                                            pEnd((ChunkHeader*) ((size_t) parent + parent->size)) {}

    Iterator begin() const { return pBegin; }

    Iterator end() const { return pEnd; }

    void validate();

};

}