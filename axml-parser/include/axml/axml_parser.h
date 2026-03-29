#pragma once

#include "axml_file.h"
#include "chunk_xml.h"

namespace axml {

class AXMLParser {

public:
    enum EventType {
        START_NAMESPACE, END_NAMESPACE,
        START_ELEMENT, END_ELEMENT
    };

private:
    template <typename T>
    void checkNode(ChunkHeader* header) {
        if (header->headerSize < sizeof(XMLNodeChunkHeader))
            throw std::runtime_error("Header is too small");
        if (header->size - header->headerSize < sizeof(T))
            throw std::runtime_error("Not enough chunk data");
    }

    template <typename T>
    T* getNode() const {
        ChunkHeader* header = rangeIt;
        return (T*) ((size_t) header + header->headerSize);
    }

    ChunkHeaderRange& range;
    StringPool& stringPool;
    ChunkHeaderRange::Iterator rangeIt;
    bool firstEl = true;

    EventType evType;

    void incrementIterator();

public:
    AXMLParser(ChunkHeaderRange& range, StringPool& stringPool) : range(range), stringPool(stringPool),
                                                                  rangeIt(range.begin()) {
    }
    AXMLParser(AXMLFile& file) : AXMLParser(file.getDocumentRange(), file.getStringPool()) {}

    bool next();

    EventType eventType() const { return evType; }

    std::string getNamespacePrefix() const { return stringPool.getString(getNode<NamespaceChunkData>()->prefix); }
    std::string getNamespaceUrl() const { return stringPool.getString(getNode<NamespaceChunkData>()->url); }

    std::string getElementNs() const { return stringPool.getString(getNode<ElementChunkData>()->ns); }
    std::string getElementName() const { return stringPool.getString(getNode<ElementChunkData>()->name); }

    AttributeChunkData* getElementAttribute(size_t i) const;
    size_t getElementAttributeCount() const { return getNode<StartElementChunkData>()->attributeCount; }
    std::string getElementAttributeNs(size_t i) const { return stringPool.getString(getElementAttribute(i)->ns); }
    std::string getElementAttributeName(size_t i) const { return stringPool.getString(getElementAttribute(i)->name); }
    ResValue& getElementAttributeTypedValue(size_t i) const { return getElementAttribute(i)->typedValue; }
    std::string getElementAttributeRawValue(size_t i) const { return stringPool.getString(getElementAttribute(i)->rawValue); }

};

}