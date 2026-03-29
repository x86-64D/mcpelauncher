#include <axml/axml_file.h>

using namespace axml;

AXMLFile::AXMLFile(void* data, size_t size) : data(data), size(size) {
    ChunkHeaderRange range((ChunkHeader*) data, (ChunkHeader*) ((size_t) data + size));
    range.validate();
    ChunkHeader* header = range.begin();
    if (header->type != ChunkHeader::TYPE_XML)
        throw std::runtime_error("Document is not XML");
    mainRange = ChunkHeaderRange(header);
    for (ChunkHeader* h : mainRange) {
        if (h->type == ChunkHeader::TYPE_STRING_POOL) {
            if (h->headerSize < sizeof(StringPoolHeader))
                throw std::runtime_error("StringPool header is not big enough");
            stringPool = StringPool((StringPoolHeader*) h);
            break;
        }
    }
}