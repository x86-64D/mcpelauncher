#pragma once

#include <fake-jni/fake-jni.h>
#include <SDL3/SDL.h>

class AssetManager : public FakeJni::JObject {
public:
    DEFINE_CLASS_NAME("android/content/res/AssetManager")

    AssetManager();
};
