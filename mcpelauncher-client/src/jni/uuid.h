#pragma once
#include <fake-jni/fake-jni.h>

class UUID : public FakeJni::JObject {
public:
    DEFINE_CLASS_NAME("java/util/UUID")

    static std::shared_ptr<UUID> randomUUID();
    static std::shared_ptr<UUID> makeRandomUUID(bool hyphens);
    std::shared_ptr<FakeJni::JString> toString();

private:
    UUID(std::shared_ptr<FakeJni::JString> uuid);

    std::shared_ptr<FakeJni::JString> uuid;
};
