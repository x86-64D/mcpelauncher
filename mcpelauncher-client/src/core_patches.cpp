#include "core_patches.h"
#include "fake_egl.h"

#include <mcpelauncher/linker.h>
#include <mcpelauncher/patch_utils.h>
#include <log.h>

CorePatches::GameWindowHandle CorePatches::currentGameWindowHandle;
std::vector<std::function<void()>> CorePatches::onWindowCreatedCallbacks;

void CorePatches::install(void* handle) {
    // void* ptr = linker::dlsym(handle, "_ZN3web4http6client7details35verify_cert_chain_platform_specificERN5boost4asio3ssl14verify_contextERKSs");
    // PatchUtils::patchCallInstruction(ptr, (void*) +[]() { return true; }, true);

    void* appPlatform = linker::dlsym(handle, "_ZTV21AppPlatform_android23");
    if(appPlatform) {
        void** vta = &((void**)appPlatform)[2];
        PatchUtils::VtableReplaceHelper vtr(handle, vta, vta);
        vtr.replace("_ZN11AppPlatform16hideMousePointerEv", &hideMousePointer);
        vtr.replace("_ZN11AppPlatform16showMousePointerEv", &showMousePointer);
    } else {
        Log::debug("CorePatches", "Failed to patch, vtable _ZTV21AppPlatform_android23 not found");
    }
}

void CorePatches::showMousePointer() {
    currentGameWindowHandle.mouseLocked = false;
    currentGameWindowHandle.callbacks->setCursorLocked(false);
}

void CorePatches::hideMousePointer() {
    currentGameWindowHandle.mouseLocked = true;
    currentGameWindowHandle.callbacks->setCursorLocked(true);
}

bool CorePatches::isMouseLocked() {
    return currentGameWindowHandle.mouseLocked;
}

void CorePatches::setFullscreen(void* t, bool fullscreen) {
    currentGameWindowHandle.callbacks->setFullscreen(fullscreen);
}

void CorePatches::setPendingDelayedPaste() {
    currentGameWindowHandle.callbacks->setDelayedPaste();
}

void CorePatches::setGameWindow(std::shared_ptr<GameWindow> gameWindow) {
    currentGameWindowHandle.window = gameWindow;
}

void CorePatches::setGameWindowCallbacks(std::shared_ptr<WindowCallbacks> gameWindowCallbacks) {
    currentGameWindowHandle.callbacks = gameWindowCallbacks;
    for(size_t i = 0; i < onWindowCreatedCallbacks.size(); i++) {
        onWindowCreatedCallbacks[i]();
    }
}

void CorePatches::loadGameWindowLibrary() {
    std::unordered_map<std::string, void*> syms;

    syms["game_window_get_primary_window"] = (void*)+[]() -> GameWindowHandle* {
        return &currentGameWindowHandle;
    };

    syms["game_window_is_mouse_locked"] = (void*)+[](GameWindowHandle* handle) -> bool {
        return handle->mouseLocked;
    };

    syms["game_window_get_input_mode"] = (void*)+[](GameWindowHandle* handle) -> int {
        return (int)handle->callbacks->getInputMode();
    };

    syms["game_window_add_keyboard_callback"] = (void*)+[](GameWindowHandle* handle, void* user, bool (*callback)(void* user, int keyCode, int action)) {
        handle->callbacks->addKeyboardCallback(user, callback);
    };

    syms["game_window_add_mouse_button_callback"] = (void*)+[](GameWindowHandle* handle, void* user, bool (*callback)(void* user, double x, double y, int button, int action)) {
        handle->callbacks->addMouseButtonCallback(user, callback);
    };

    syms["game_window_add_mouse_position_callback"] = (void*)+[](GameWindowHandle* handle, void* user, bool (*callback)(void* user, double x, double y, bool relative)) {
        handle->callbacks->addMousePositionCallback(user, callback);
    };

    syms["game_window_add_mouse_scroll_callback"] = (void*)+[](GameWindowHandle* handle, void* user, bool (*callback)(void* user, double x, double y, double dx, double dy)) {
        handle->callbacks->addMouseScrollCallback(user, callback);
    };

    syms["game_window_add_window_creation_callback"] = (void*)+[](void* user, void (*onCreated)(void* user)) {
        onWindowCreatedCallbacks.emplace_back(std::bind(onCreated, user));
    };

    syms["game_window_add_swap_buffers_callback"] = (void*)+[](void* user, void (*callback)(void* user, EGLDisplay display, EGLSurface surface)) {
        FakeEGL::addSwapBuffersCallback(user, callback);
    };

    linker::load_library("libmcpelauncher_gamewindow.so", syms);
}
