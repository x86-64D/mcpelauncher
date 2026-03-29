#pragma once

#include <fake-jni/fake-jni.h>
#include <SDL3/SDL.h>
#include "asset_manager.h"

class FMOD : public FakeJni::JObject {
public:
    DEFINE_CLASS_NAME("org/fmod/FMOD")

    FMOD();
    ~FMOD();

    static FakeJni::JBoolean checkInit();
    static FakeJni::JBoolean supportsAAudio();
    static FakeJni::JBoolean supportsLowLatency();

    static std::shared_ptr<AssetManager> getAssetManager();
};
