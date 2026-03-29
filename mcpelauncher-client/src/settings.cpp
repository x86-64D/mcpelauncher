#include "settings.h"
#include <properties/property_list.h>
#include <properties/property.h>
#include <mcpelauncher/path_helper.h>
#include <fstream>

std::optional<bool> Settings::enable_imgui = {};
std::atomic<int> Settings::menubarsize = 0;
std::string Settings::clipboard;
bool Settings::enable_keyboard_autofocus_patches_1_20_60 = false;
bool Settings::enable_keyboard_autofocus_paste_patches_1_20_60 = false;
bool Settings::enable_intel_sprint_strafe_patch = false;
bool Settings::enable_menubar = true;

int Settings::enable_fps_hud;
float Settings::fps_hud_x;
float Settings::fps_hud_y;

int Settings::enable_keystroke_hud;
float Settings::keystroke_hud_x;
float Settings::keystroke_hud_y;

std::string Settings::videoMode;
float Settings::scale;
std::string Settings::menubarFocusKey;
bool Settings::fullscreen;
bool Settings::vsync;

char GameOptions::leftKey = 'A';
char GameOptions::downKey = 'S';
char GameOptions::rightKey = 'D';
char GameOptions::upKey = 'W';

char GameOptions::leftKeyFullKeyboard = 'A';
char GameOptions::downKeyFullKeyboard = 'S';
char GameOptions::rightKeyFullKeyboard = 'D';
char GameOptions::upKeyFullKeyboard = 'W';

bool GameOptions::fullKeyboard = false;

static properties::property_list settings('=');
static properties::property<std::string> enable_imgui(settings, "enable_imgui", "auto");
static properties::property<bool> enable_keyboard_autofocus_patches_1_20_60(settings, "enable_keyboard_autofocus_patches_1_20_60", /* default if not defined*/ false);              // should be enabled on sdl3 builds by default
static properties::property<bool> enable_keyboard_autofocus_paste_patches_1_20_60(settings, "enable_keyboard_autofocus_paste_patches_1_20_60", /* default if not defined*/ false);  // should be enabled on sdl3 builds by default
static properties::property<bool> enable_intel_sprint_strafe_patch(settings, "enable_intel_sprint_strafe_patch", /* default if not defined*/ false);
static properties::property<bool> enable_menubar(settings, "enable_menubar", /* default if not defined*/ true);

static properties::property<int> enable_fps_hud(settings, "enable_fps_hud", /* default if not defined*/ false);
static properties::property<float> fps_hud_x(settings, "fps_hud_x", /* default if not defined*/ 0);
static properties::property<float> fps_hud_y(settings, "fps_hud_y", /* default if not defined*/ 0);

static properties::property<int> enable_keystroke_hud(settings, "enable_keystroke_hud", /* default if not defined*/ false);
static properties::property<float> keystroke_hud_x(settings, "keystroke_hud_x", /* default if not defined*/ 0);
static properties::property<float> keystroke_hud_y(settings, "keystroke_hud_y", /* default if not defined*/ 0);

static properties::property<std::string> videoMode(settings, "videoMode", "");
static properties::property<float> scale(settings, "scale", 1);
static properties::property<std::string> menubarFocusKey(settings, "menubarFocusKey", "");
static properties::property<bool> fullscreen(settings, "fullscreen", /* default if not defined*/ false);
static properties::property<bool> vsync(settings, "vsync", /* default if not defined*/ true);

std::string Settings::getPath() {
    return PathHelper::getPrimaryDataDirectory() + "mcpelauncher-client-settings.txt";
}

void Settings::load() {
    std::ifstream propertiesFile(getPath());
    if(propertiesFile) {
        settings.load(propertiesFile);
    }
    if(::enable_imgui.get() != "auto") {
        properties::property_list dummy;
        properties::property<bool> dummy_enable_imgui(dummy, "enable_imgui", false);
        dummy_enable_imgui.parse_value(::enable_imgui.get());
        Settings::enable_imgui = dummy_enable_imgui.get();
    }
    Settings::enable_keyboard_autofocus_patches_1_20_60 = ::enable_keyboard_autofocus_patches_1_20_60.get();
    Settings::enable_keyboard_autofocus_paste_patches_1_20_60 = ::enable_keyboard_autofocus_paste_patches_1_20_60.get();
    Settings::enable_intel_sprint_strafe_patch = ::enable_intel_sprint_strafe_patch.get();
    Settings::enable_menubar = ::enable_menubar.get();

    Settings::enable_fps_hud = ::enable_fps_hud.get();
    Settings::fps_hud_x = ::fps_hud_x.get();
    Settings::fps_hud_y = ::fps_hud_y.get();

    Settings::enable_keystroke_hud = ::enable_keystroke_hud.get();
    Settings::keystroke_hud_x = ::keystroke_hud_x.get();
    Settings::keystroke_hud_y = ::keystroke_hud_y.get();

    Settings::videoMode = ::videoMode.get();
    Settings::scale = ::scale.get();
    Settings::menubarFocusKey = ::menubarFocusKey.get();
    Settings::fullscreen = ::fullscreen.get();
    Settings::vsync = ::vsync.get();
}

void Settings::save() {
    if(Settings::enable_imgui.has_value()) {
        ::enable_imgui.set(Settings::enable_imgui.value() ? "true" : "false");
    } else {
        ::enable_imgui.set("auto");
    }
    ::enable_keyboard_autofocus_patches_1_20_60.set(Settings::enable_keyboard_autofocus_patches_1_20_60);
    ::enable_keyboard_autofocus_paste_patches_1_20_60.set(Settings::enable_keyboard_autofocus_paste_patches_1_20_60);
    ::enable_intel_sprint_strafe_patch.set(Settings::enable_intel_sprint_strafe_patch);
    ::enable_menubar.set(Settings::enable_menubar);

    ::enable_fps_hud.set(Settings::enable_fps_hud);
    ::fps_hud_x.set(Settings::fps_hud_x);
    ::fps_hud_y.set(Settings::fps_hud_y);

    ::enable_keystroke_hud.set(Settings::enable_keystroke_hud);
    ::keystroke_hud_x.set(Settings::keystroke_hud_x);
    ::keystroke_hud_y.set(Settings::keystroke_hud_y);

    ::videoMode.set(Settings::videoMode);
    ::scale.set(Settings::scale);
    ::menubarFocusKey.set(Settings::menubarFocusKey);
    std::ofstream propertiesFile(getPath());
    ::fullscreen.set(Settings::fullscreen);
    ::vsync.set(Settings::vsync);
    if(propertiesFile) {
        settings.save(propertiesFile);
    }
}
