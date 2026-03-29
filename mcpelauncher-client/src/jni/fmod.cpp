#include "fmod.h"

FMOD::FMOD() {
}

FMOD::~FMOD() {
}

FakeJni::JBoolean FMOD::checkInit() {
    return true;
}

FakeJni::JBoolean FMOD::supportsAAudio() {
#ifdef HAVE_SDL3AUDIO
    return true;
#else
    return false;
#endif
}

FakeJni::JBoolean FMOD::supportsLowLatency() {
    return true;
}

std::shared_ptr<AssetManager> FMOD::getAssetManager() {
    return std::make_shared<AssetManager>(AssetManager());
}
