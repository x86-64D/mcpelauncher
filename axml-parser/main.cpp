#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <axml/axml_file.h>
#include <axml/axml_parser.h>

int main(int argc, const char* argv[]) {
    const char* path = argv[1];
    int fd = open(path, O_RDONLY);
    auto size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    if (size < 0)
        throw std::runtime_error("Failed to get file size");
    void* ptr = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
    axml::AXMLFile file (ptr, size);
    axml::AXMLParser parser (file);
    while (parser.next()) {
        if (parser.eventType() == axml::AXMLParser::START_NAMESPACE) {
            printf("START_NAMESPACE prefix=%s url=%s\n", parser.getNamespacePrefix().c_str(),
                   parser.getNamespaceUrl().c_str());
        } else if (parser.eventType() == axml::AXMLParser::END_NAMESPACE) {
            printf("END_NAMESPACE prefix=%s url=%s\n", parser.getNamespacePrefix().c_str(),
                   parser.getNamespaceUrl().c_str());
        } else if (parser.eventType() == axml::AXMLParser::START_ELEMENT) {
            printf("START_ELEMENT ns=%s name=%s\n", parser.getElementNs().c_str(), parser.getElementName().c_str());
            size_t attrCount = parser.getElementAttributeCount();
            for (size_t i = 0; i < attrCount; i++) {
                auto& typedValue = parser.getElementAttributeTypedValue(i);
                printf(" ATTRIBUTE ns=%s name=%s ", parser.getElementAttributeNs(i).c_str(),
                       parser.getElementAttributeName(i).c_str());
                if (typedValue.isInteger()) {
                    printf("typedValue=%i\n", typedValue.data);
                } else {
                    printf("rawValue=%s\n", parser.getElementAttributeRawValue(i).c_str());
                }
            }
        } else if (parser.eventType() == axml::AXMLParser::END_ELEMENT) {
            printf("END_ELEMENT ns=%s name=%s\n", parser.getElementNs().c_str(), parser.getElementName().c_str());
        }
    }
    munmap(ptr, size);
    return 0;
}
