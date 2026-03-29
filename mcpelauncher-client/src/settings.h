#pragma once
#include <atomic>
#include <string>
#include <optional>

struct Settings {
    static std::optional<bool> enable_imgui;
    static std::atomic<int> menubarsize;
    static std::string clipboard;
    static bool enable_keyboard_autofocus_patches_1_20_60;
    static bool enable_keyboard_autofocus_paste_patches_1_20_60;
    static bool enable_intel_sprint_strafe_patch;
    static bool enable_menubar;

    static int enable_fps_hud;
    static float fps_hud_x;
    static float fps_hud_y;

    static int enable_keystroke_hud;
    static float keystroke_hud_x;
    static float keystroke_hud_y;

    static std::string videoMode;
    static float scale;
    static std::string menubarFocusKey;

    static bool fullscreen;
    static bool vsync;

    static std::string getPath();
    static void load();
    static void save();
};

struct GameOptions {
    static char leftKey;
    static char downKey;
    static char rightKey;
    static char upKey;

    static char leftKeyFullKeyboard;
    static char downKeyFullKeyboard;
    static char rightKeyFullKeyboard;
    static char upKeyFullKeyboard;

    static bool fullKeyboard;
};