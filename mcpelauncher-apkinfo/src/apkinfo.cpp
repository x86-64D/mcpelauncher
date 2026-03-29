#include <mcpelauncher/apkinfo.h>

ApkInfo ApkInfo::fromXml(axml::AXMLParser& parser) {
    std::vector<std::string> path;
    ApkInfo ret;
    while (parser.next()) {
        if (parser.eventType() == axml::AXMLParser::START_ELEMENT) {
            path.push_back(parser.getElementName());

            if (path.size() == 1 && path[0] == "manifest") {
                for (size_t i = 0; i < parser.getElementAttributeCount(); i++) {
                    auto aName = parser.getElementAttributeName(i);
                    if (aName == "versionCode") {
                        ret.versionCode = parser.getElementAttributeTypedValue(i).data;
                    } else if (aName == "versionName") {
                        ret.versionName = parser.getElementAttributeRawValue(i);
                    } else if (aName == "package") {
                        ret.package = parser.getElementAttributeRawValue(i);
                    } else if (aName == "split") {
                        ret.split = parser.getElementAttributeRawValue(i);
                    }
                }
            }
        } else if (parser.eventType() == axml::AXMLParser::END_ELEMENT) {
            path.pop_back();
        }
    }
    return ret;
}