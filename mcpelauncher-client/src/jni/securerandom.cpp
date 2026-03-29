#include "securerandom.h"
#ifndef NO_OPENSSL
#include <openssl/rand.h>
#endif

std::shared_ptr<FakeJni::JByteArray> SecureRandom::GenerateRandomBytes(int bytes) {
    auto randomBytes = std::make_shared<FakeJni::JByteArray>(bytes);
#ifndef NO_OPENSSL
    RAND_bytes((unsigned char *)randomBytes->getArray(), bytes);
#endif
    return randomBytes;
}
