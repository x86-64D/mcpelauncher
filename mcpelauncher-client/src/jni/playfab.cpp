#include "uuid.h"
#include "playfab.h"

std::shared_ptr<FakeJni::JString> AndroidJniHelperMultiplayer::createUUID() {
    return UUID::makeRandomUUID(true)->toString();
}

std::shared_ptr<FakeJni::JArray<FakeJni::JString>> EventTracerHelperMultiplayer::getPlayFabEventCommonFields(std::shared_ptr<FakeJni::JString> s) {
    auto res = std::make_shared<FakeJni::JArray<FakeJni::JString>>(12);
    res->Set(0, std::make_shared<FakeJni::JString>("OSName"));
    res->Set(1, std::make_shared<FakeJni::JString>("Android"));
    res->Set(2, std::make_shared<FakeJni::JString>("OSVersion"));
    res->Set(3, std::make_shared<FakeJni::JString>("12"));
    res->Set(4, std::make_shared<FakeJni::JString>("DeviceMake"));
    res->Set(5, std::make_shared<FakeJni::JString>("Linux"));
    res->Set(6, std::make_shared<FakeJni::JString>("DeviceModel"));
    res->Set(7, std::make_shared<FakeJni::JString>("Linux"));
    res->Set(8, std::make_shared<FakeJni::JString>("AppName"));
    res->Set(9, std::make_shared<FakeJni::JString>("Minecraft"));
    res->Set(10, std::make_shared<FakeJni::JString>("AppVersion"));
    res->Set(11, std::make_shared<FakeJni::JString>("1.0.0"));
    return res;
}