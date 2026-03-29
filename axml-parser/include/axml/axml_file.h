#pragma once

#include <istream>
#include <vector>
#include "endian_aware_int.h"
#include "string_pool.h"
#include "chunk_header_range.h"

namespace axml {

class AXMLFile {

private:
    void* data;
    size_t size;

    ChunkHeaderRange mainRange;
    StringPool stringPool;

public:
    AXMLFile(void* data, size_t size);

    StringPool& getStringPool() { return stringPool; }

    ChunkHeaderRange& getDocumentRange() { return mainRange; }

};

}