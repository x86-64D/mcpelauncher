#include <axml/chunk_header_range.h>
#include <stdexcept>

using namespace axml;

void ChunkHeaderRange::validate() {
    for (ChunkHeader* header : *this) {
        if (header->headerSize.get() < sizeof(ChunkHeader) ||
            (size_t) header + header->headerSize > (size_t) pEnd ||
            (size_t) header + header->size > (size_t) pEnd)
            throw std::out_of_range("Invalid chunk size");
        if (header->type == ChunkHeader::TYPE_XML) {
            ChunkHeaderRange(header).validate();
        }
    }
}