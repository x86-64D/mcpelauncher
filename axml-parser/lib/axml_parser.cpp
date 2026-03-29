#include <axml/axml_parser.h>

using namespace axml;

void AXMLParser::incrementIterator() {
    if (firstEl)
        firstEl = false;
    else if (rangeIt != range.end())
        ++rangeIt;
}

bool AXMLParser::next() {
    while (true) {
        incrementIterator();
        if (rangeIt == range.end())
            return false;

        ChunkHeader* header = rangeIt;
        if (header->type == ChunkHeader::TYPE_XML_START_NAMESPACE ||
                header->type == ChunkHeader::TYPE_XML_END_NAMESPACE) {
            checkNode<NamespaceChunkData>(header);
            evType = header->type == ChunkHeader::TYPE_XML_START_NAMESPACE
                     ? EventType::START_NAMESPACE : EventType::END_NAMESPACE;
            return true;
        }
        if (header->type == ChunkHeader::TYPE_XML_START_ELEMENT) {
            checkNode<StartElementChunkData>(header);
            StartElementChunkData* data = getNode<StartElementChunkData>();
            if (data->attributeSize < sizeof(AttributeChunkData))
                throw std::runtime_error("Invalid attribute list item size");
            if (header->headerSize + data->attributeStart + data->attributeSize * data->attributeCount > header->size)
                throw std::runtime_error("Invalid attribute list size");
            evType = EventType::START_ELEMENT;
            return true;
        }
        if (header->type == ChunkHeader::TYPE_XML_END_ELEMENT) {
            checkNode<ElementChunkData>(header);
            evType = EventType::END_ELEMENT;
            return true;
        }
    }
}

AttributeChunkData* AXMLParser::getElementAttribute(size_t i) const {
    StartElementChunkData* data = getNode<StartElementChunkData>();
    if (i >= data->attributeCount)
        throw std::out_of_range("Invalid attribute index");
    return (AttributeChunkData*) ((size_t) data + data->attributeStart + data->attributeSize * i);
}