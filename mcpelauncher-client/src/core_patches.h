#pragma once

#include <memory>
#include <vector>
#include <game_window.h>
#include "window_callbacks.h"

class CorePatches {
private:
    struct GameWindowHandle {
        std::shared_ptr<GameWindow> window;
        std::shared_ptr<WindowCallbacks> callbacks;
        bool mouseLocked = false;
    };

    static GameWindowHandle currentGameWindowHandle;

    static std::vector<std::function<void()>> onWindowCreatedCallbacks;

public:
    static void
    install(void* handle);

    static void showMousePointer();

    static void hideMousePointer();

    static bool isMouseLocked();

    static void setPendingDelayedPaste();

    static void setFullscreen(void* t, bool fullscreen);

    static void setGameWindow(std::shared_ptr<GameWindow> gameWindow);

    static void setGameWindowCallbacks(std::shared_ptr<WindowCallbacks> gameWindowCallbacks);

    static void loadGameWindowLibrary();
};
