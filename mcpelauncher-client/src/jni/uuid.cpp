#include "uuid.h"
#include <random>
#include <climits>

UUID::UUID(std::shared_ptr<FakeJni::JString> uuid) : uuid(uuid) {
}

std::shared_ptr<UUID> UUID::randomUUID() {
    return makeRandomUUID(true);
}

std::shared_ptr<UUID> UUID::makeRandomUUID(bool hyphens) {
    static std::independent_bits_engine<std::random_device, CHAR_BIT, unsigned char> engine;
    unsigned char rawBytes[16];
    std::generate(rawBytes, rawBytes + 16, std::ref(engine));
    rawBytes[6] = (rawBytes[6] & (unsigned char)0x0Fu) | (unsigned char)0x40u;
    rawBytes[8] = (rawBytes[6] & (unsigned char)0x3Fu) | (unsigned char)0x80u;
    std::string ret;
    ret.resize(hyphens ? 37 : 33);
    snprintf((char*)ret.c_str(), hyphens ? 37 : 33, hyphens ? "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x" : "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
             rawBytes[0], rawBytes[1], rawBytes[2], rawBytes[3],
             rawBytes[4], rawBytes[5], rawBytes[6], rawBytes[7], rawBytes[8], rawBytes[9],
             rawBytes[10], rawBytes[11], rawBytes[12], rawBytes[13], rawBytes[14], rawBytes[15]);
    ret.resize(hyphens ? 36 : 32);
    return std::shared_ptr<UUID>(new UUID(std::make_shared<FakeJni::JString>(ret)));
}

std::shared_ptr<FakeJni::JString> UUID::toString() {
    return uuid;
}
