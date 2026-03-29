#include "window_sdl3.h"
#include "game_window_manager.h"

#include <codecvt>
#include <iomanip>
#include <thread>
#include <sstream>

#include <math.h>
#include <SDL3/SDL.h>

struct SDL_X11CursorData {
    void* cursor;
};
struct SDL_X11Cursor {
    SDL_X11Cursor* next;
    SDL_X11CursorData* internal;
};

SDL3GameWindow::SDL3GameWindow(const std::string& title, int width, int height, GraphicsApi api) : GameWindow(title, width, height, api), width(width), height(height), windowedWidth(width), windowedHeight(height) {
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
    SDL_SetHint(SDL_HINT_APP_NAME, "Minecraft");
    if(api == GraphicsApi::OPENGL_ES2) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    } else if(api == GraphicsApi::OPENGL) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    }
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    window = SDL_CreateWindow(title.data(), width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if(window == nullptr) {
        // Throw an exception, otherwise it would crash due to a nullptr without any information
        const char* error = SDL_GetError();
        throw std::runtime_error(error == nullptr ? "SDL3 failed to create a window without any error message" : error);
    }
    context = SDL_GL_CreateContext(window);
    if(context == nullptr) {
        SDL_DestroyWindow(window);
        const char* error = SDL_GetError();
        throw std::runtime_error(error == nullptr ? "SDL3 failed to create a window context without any error message" : error);
    }
    SDL_GL_MakeCurrent(window, context);

    // SDL_SetHint(SDL_HINT_ENABLE_SCREEN_KEYBOARD, "0");
    SDL_StopTextInput(window);
    setRelativeScale();

    // HACK: Force SDL to not alter the cursor on x11, fixing cursor scaling issues
    if(strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
        SDL_X11Cursor* def = (SDL_X11Cursor*)SDL_GetDefaultCursor();
        def->internal->cursor = NULL;
    }
}

void SDL3GameWindow::makeCurrent(bool c) {
    SDL_GL_MakeCurrent(window, c ? context : nullptr);
}

SDL3GameWindow::~SDL3GameWindow() {
    if(window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}

void SDL3GameWindow::setIcon(std::string const& iconPath) {
}

void SDL3GameWindow::setRelativeScale() {
    int fx, fy;
    SDL_GetWindowSizeInPixels(window, &fx, &fy);

    int wx, wy;
    SDL_GetWindowSize(window, &wx, &wy);

    relativeScale = (int)floor(((fx / wx) + (fy / wy)) / 2);
    relativeScaleX = (double)fx / wx;
    relativeScaleY = (double)fy / wy;
    // Update window size to match content size mismatch
    width = fx;
    height = fy;
    resized = true;
}

int SDL3GameWindow::getRelativeScale() const {
    return relativeScale;
}

void SDL3GameWindow::getWindowSize(int& width, int& height) const {
    width = this->width;
    height = this->height;
}

void SDL3GameWindow::show() {
    SDL_ShowWindow(window);
}

void SDL3GameWindow::close() {
    if(window) {
        onClose();
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}

static GamepadButtonId getKeyGamePad(int btn) {
    switch(btn) {
    case SDL_GAMEPAD_BUTTON_SOUTH:
        return GamepadButtonId::A;
    case SDL_GAMEPAD_BUTTON_EAST:
        return GamepadButtonId::B;
    case SDL_GAMEPAD_BUTTON_WEST:
        return GamepadButtonId::X;
    case SDL_GAMEPAD_BUTTON_NORTH:
        return GamepadButtonId::Y;
    case SDL_GAMEPAD_BUTTON_BACK:
        return GamepadButtonId::BACK;
    case SDL_GAMEPAD_BUTTON_GUIDE:
        return GamepadButtonId::GUIDE;
    case SDL_GAMEPAD_BUTTON_START:
        return GamepadButtonId::START;
    case SDL_GAMEPAD_BUTTON_LEFT_STICK:
        return GamepadButtonId::LEFT_STICK;
    case SDL_GAMEPAD_BUTTON_RIGHT_STICK:
        return GamepadButtonId::RIGHT_STICK;
    case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
        return GamepadButtonId::LB;
    case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
        return GamepadButtonId::RB;
    case SDL_GAMEPAD_BUTTON_DPAD_UP:
        return GamepadButtonId::DPAD_UP;
    case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
        return GamepadButtonId::DPAD_DOWN;
    case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
        return GamepadButtonId::DPAD_LEFT;
    case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
        return GamepadButtonId::DPAD_RIGHT;
    default:
        return GamepadButtonId::UNKNOWN;
    }
}

static GamepadAxisId getAxisGamepad(int btn) {
    switch(btn) {
    case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
        return GamepadAxisId::LEFT_TRIGGER;
    case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
        return GamepadAxisId::RIGHT_TRIGGER;
    case SDL_GAMEPAD_AXIS_LEFTX:
        return GamepadAxisId::LEFT_X;
    case SDL_GAMEPAD_AXIS_LEFTY:
        return GamepadAxisId::LEFT_Y;
    case SDL_GAMEPAD_AXIS_RIGHTX:
        return GamepadAxisId::RIGHT_X;
    case SDL_GAMEPAD_AXIS_RIGHTY:
        return GamepadAxisId::RIGHT_Y;
    default:
        return GamepadAxisId::UNKNOWN;
    }
}

static int getMouseButton(int btn) {
    if(btn >= SDL_BUTTON_X1) {
        // To match glfw codebase
        return 8 + btn - SDL_BUTTON_X1;
    }
    switch(btn) {
    case SDL_BUTTON_LEFT:
        return 1;
    case SDL_BUTTON_RIGHT:
        return 2;
    case SDL_BUTTON_MIDDLE:
        return 3;
    default:
        return 0;
    }
}

static std::string getModeDescription(const SDL_DisplayMode* mode) {
    std::stringstream desc;
    desc << mode->w << "x" << mode->h << " @ " << mode->refresh_rate << " * " << mode->pixel_density;
    return desc.str();
}

void SDL3GameWindow::pollEvents() {
    if(requestedWindowMode != None) {
        SDL_SetWindowFullscreen(window, requestedWindowMode == RequestWindowMode::Fullscreen);
        requestedWindowMode = RequestWindowMode::None;
    }
    if(pendingFullscreenModeSwitch) {
        pendingFullscreenModeSwitch = false;
        if(mode.id == -1) {
            SDL_SetWindowFullscreenMode(window, NULL);
        } else {
            int nModes = 0;
            auto display = SDL_GetDisplayForWindow(window);
            auto modes = SDL_GetFullscreenDisplayModes(display, &nModes);
            if(nModes > mode.id && mode.description == getModeDescription(modes[mode.id])) {
                SDL_SetWindowFullscreenMode(window, modes[mode.id]);
            }
        }
    }
    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
        switch(ev.type) {
        case SDL_EVENT_MOUSE_MOTION:
            if(!SDL_GetWindowRelativeMouseMode(window)) {
                onMousePosition(ev.motion.x * relativeScaleX, ev.motion.y * relativeScaleY);
            } else {
                onMouseRelativePosition(ev.motion.xrel, ev.motion.yrel);
            }
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            onMouseScroll(ev.wheel.mouse_x * relativeScaleX, ev.wheel.mouse_y * relativeScaleY, ev.wheel.x, ev.wheel.y);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            onMouseButton(ev.button.x * relativeScaleX, ev.button.y * relativeScaleY, getMouseButton(ev.button.button), ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN ? MouseButtonAction::PRESS : MouseButtonAction::RELEASE);
            break;
        case SDL_EVENT_FINGER_DOWN: {
            int w, h;
            getWindowSize(w, h);
            onTouchStart(ev.tfinger.fingerID, ev.tfinger.x * w, ev.tfinger.y * h);
            break;
        }
        case SDL_EVENT_FINGER_UP: {
            int w, h;
            getWindowSize(w, h);
            onTouchEnd(ev.tfinger.fingerID, ev.tfinger.x * w, ev.tfinger.y * h);
            break;
        }
        case SDL_EVENT_FINGER_MOTION: {
            int w, h;
            getWindowSize(w, h);
            onTouchUpdate(ev.tfinger.fingerID, ev.tfinger.x * w, ev.tfinger.y * h);
            break;
        }
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            if(ev.type == SDL_EVENT_KEY_DOWN) {
                if(SDL_TextInputActive(window)) {
                    if(ev.key.key == SDLK_BACKSPACE) {
                        onKeyboardText("\b");
                    } else if(ev.key.key == SDLK_DELETE) {
                        onKeyboardText("\x7F");
                    } else if(ev.key.key == SDLK_RETURN) {
                        onKeyboardText("\n");
                    }
                } else if(ev.key.key < 0x40000000) {
                    onKeyboardText(std::string(1, (char)ev.key.key));
                }
            }

            SDL_Keymod mods;
            mods = SDL_GetModState();
            if(mods & SDL_KMOD_CTRL && ev.key.key == SDLK_V && ev.type == SDL_EVENT_KEY_DOWN) {
                auto str = SDL_GetClipboardText();
                onPaste(str);
            }
            onKeyboard(getKeyMinecraft(SDL_GetKeyFromScancode(ev.key.scancode, SDL_KMOD_NONE, false)), ev.type == SDL_EVENT_KEY_DOWN ? ev.key.repeat ? KeyAction::REPEAT : KeyAction::PRESS : KeyAction::RELEASE, translateMeta(mods));
            break;
        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        case SDL_EVENT_GAMEPAD_BUTTON_UP:
            onGamepadButton(ev.gbutton.which, getKeyGamePad(ev.gbutton.button), ev.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN);
            break;
        case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            onGamepadAxis(ev.gaxis.which, getAxisGamepad(ev.gaxis.axis), (float)ev.gaxis.value / 32767.0f);
            break;
        case SDL_EVENT_GAMEPAD_ADDED:
        case SDL_EVENT_GAMEPAD_REMOVED: {
            if(ev.type == SDL_EVENT_GAMEPAD_ADDED) {
                SDL_OpenGamepad(ev.gdevice.which);
            }
            onGamepadState(ev.gdevice.which, ev.type == SDL_EVENT_GAMEPAD_ADDED);
            if(ev.type == SDL_EVENT_GAMEPAD_REMOVED) {
                SDL_CloseGamepad(SDL_GetGamepadFromID(ev.gdevice.which));
            }
            break;
        }
        case SDL_EVENT_WINDOW_RESIZED:
            setRelativeScale();
            if(cursorDisabled) {
                const SDL_Rect* currentMouseRect = SDL_GetWindowMouseRect(window);
                if(currentMouseRect != NULL && (currentMouseRect->x > width || currentMouseRect->y > height)) {
                    SDL_Rect r = SDL_Rect{.x = width / 2, .y = height / 2, .w = 1, .h = 1};
                    SDL_SetWindowMouseRect(window, &r);
                    SDL_WarpMouseInWindow(window, width / 2, height / 2);
                }
            }
            break;
        case SDL_EVENT_TEXT_INPUT:
            onKeyboardText(ev.text.text ? ev.text.text : "");
            break;
        case SDL_EVENT_DROP_FILE:
            onDrop(ev.drop.data);
            break;
        case SDL_EVENT_DROP_TEXT:
            onPaste(ev.drop.data);
            break;
        // case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        case SDL_EVENT_QUIT:
            onClose();
            break;
        case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
            modes.clear();
            setRelativeScale();
            break;
        case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
            setRelativeScale();
            break;
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            if(cursorDisabled) {
                float x, y;
                SDL_GetGlobalMouseState(&x, &y);
                int windowX, windowY;
                SDL_GetWindowPosition(window, &windowX, &windowY);
                SDL_Rect r = {.x = (int)x - windowX, .y = (int)y - windowY, .w = 1, .h = 1};
                SDL_SetWindowMouseRect(window, &r);
                SDL_SetWindowRelativeMouseMode(window, true);
            }
            break;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
            if(cursorDisabled) {
                SDL_SetWindowRelativeMouseMode(window, false);
                SDL_SetWindowMouseRect(window, NULL);
            }
            if(centerMouseNextEnable) {
                centerMouseNextEnable = false;
            }
            break;
        default:
            break;
        }
    }
    if(resized) {
        onWindowSizeChanged(width, height);
    }
}

bool SDL3GameWindow::getCursorDisabled() {
    return SDL_GetWindowRelativeMouseMode(window);
}

void SDL3GameWindow::setCursorDisabled(bool disabled) {
    if(disabled == cursorDisabled) {
        return;
    }
    cursorDisabled = disabled;
    SDL_WindowFlags flags = SDL_GetWindowFlags(window);
    if(flags & SDL_WINDOW_INPUT_FOCUS) {
        if(disabled) {
            if(isMouseInWindow()) {
                float x, y;
                SDL_GetMouseState(&x, &y);
                SDL_Rect r = {.x = (int)x, .y = (int)y, .w = 1, .h = 1};
                SDL_SetWindowMouseRect(window, &r);
            } else {
                SDL_WarpMouseInWindow(window, width / 2, height / 2);
                SDL_Rect r = {.x = width / 2, .y = height / 2, .w = 1, .h = 1};
                SDL_SetWindowMouseRect(window, &r);
                centerMouseNextEnable = true;
            }
        } else {
            SDL_SetWindowMouseRect(window, NULL);
            if(getenv("GAMEWINDOW_CENTER_CURSOR") || centerMouseNextEnable) {
                SDL_WarpMouseInWindow(window, width / 2, height / 2);
                centerMouseNextEnable = false;
            }
        }
        SDL_SetWindowRelativeMouseMode(window, disabled);
    }
}

void SDL3GameWindow::setFullscreenMode(const FullscreenMode& mode) {
    this->mode = mode;
    pendingFullscreenModeSwitch = true;
}

FullscreenMode SDL3GameWindow::getFullscreenMode() {
    auto display = SDL_GetDisplayForWindow(window);
    int nModes;
    auto modes = SDL_GetFullscreenDisplayModes(display, &nModes);
    auto mode = SDL_GetWindowFullscreenMode(window);
    if(mode && modes) {
        auto desc = getModeDescription(mode);
        for(int i = 0; i < nModes; i++) {
            if(desc == getModeDescription(modes[i])) {
                return FullscreenMode{.id = i, .description = desc};
            }
        }
    }
    return FullscreenMode{-1};
}

std::vector<FullscreenMode> SDL3GameWindow::getFullscreenModes() {
    if(modes.empty()) {
        int nModes = 0;
        auto display = SDL_GetDisplayForWindow(window);
        auto modes = SDL_GetFullscreenDisplayModes(display, &nModes);
        for(int j = 0; j < nModes; j++) {
            this->modes.emplace_back(FullscreenMode{.id = j, .description = getModeDescription(modes[j])});
        }
    }
    return modes;
}

bool SDL3GameWindow::getFullscreen() {
    return SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN;
}

void SDL3GameWindow::setFullscreen(bool fullscreen) {
    if(fullscreen)
        requestedWindowMode = RequestWindowMode::Fullscreen;
    else
        requestedWindowMode = RequestWindowMode::Windowed;
}

void SDL3GameWindow::setClipboardText(std::string const& text) {
    SDL_SetClipboardText(text.data());
}

void SDL3GameWindow::swapBuffers() {
    SDL_GL_SwapWindow(window);
}

void SDL3GameWindow::setSwapInterval(int interval) {
    SDL_GL_SetSwapInterval(interval);
}

void SDL3GameWindow::startTextInput() {
    // SDL_StopTextInput();
    // SDL_SetHint(SDL_HINT_ENABLE_SCREEN_KEYBOARD, "1");
    // SDL_SetHint(SDL_HINT_IME_INTERNAL_EDITING, "1");
    SDL_StartTextInput(window);
}

void SDL3GameWindow::stopTextInput() {
    SDL_StopTextInput(window);
    // SDL_SetHint(SDL_HINT_ENABLE_SCREEN_KEYBOARD, "0");
    // SDL_StartTextInput();
}

bool SDL3GameWindow::isMouseInWindow() {
    int windowX, windowY;
    SDL_GetWindowPosition(window, &windowX, &windowY);
    float mouseX, mouseY;
    SDL_GetGlobalMouseState(&mouseX, &mouseY);
    return (mouseX >= windowX && mouseY >= windowY && mouseX <= windowX + width && mouseY <= windowY + height);
}

KeyCode SDL3GameWindow::getKeyMinecraft(int keyCode) {
    if(keyCode >= SDLK_F1 && keyCode <= SDLK_F12)
        return (KeyCode)(keyCode - SDLK_F1 + (int)KeyCode::FN1);
    if(keyCode >= SDLK_KP_1 && keyCode <= SDLK_KP_9)
        return (KeyCode)(keyCode - SDLK_KP_1 + (int)KeyCode::NUMPAD_1);
    if(keyCode >= SDLK_A && keyCode <= SDLK_Z)
        return (KeyCode)(keyCode - SDLK_A + (int)KeyCode::A);
    switch(keyCode) {
    case SDLK_BACKSPACE:
        return KeyCode::BACKSPACE;
    case SDLK_TAB:
        return KeyCode::TAB;
    case SDLK_RETURN:
        return KeyCode::ENTER;
    case SDLK_LSHIFT:
        return KeyCode::LEFT_SHIFT;
    case SDLK_RSHIFT:
        return KeyCode::RIGHT_SHIFT;
    case SDLK_LCTRL:
        return KeyCode::LEFT_CTRL;
    case SDLK_RCTRL:
        return KeyCode::RIGHT_CTRL;
    case SDLK_PAUSE:
        return KeyCode::PAUSE;
    case SDLK_CAPSLOCK:
        return KeyCode::CAPS_LOCK;
    case SDLK_ESCAPE:
        return KeyCode::ESCAPE;
    case SDLK_PAGEUP:
        return KeyCode::PAGE_UP;
    case SDLK_PAGEDOWN:
        return KeyCode::PAGE_DOWN;
    case SDLK_END:
        return KeyCode::END;
    case SDLK_HOME:
        return KeyCode::HOME;
    case SDLK_LEFT:
        return KeyCode::LEFT;
    case SDLK_UP:
        return KeyCode::UP;
    case SDLK_RIGHT:
        return KeyCode::RIGHT;
    case SDLK_DOWN:
        return KeyCode::DOWN;
    case SDLK_INSERT:
        return KeyCode::INSERT;
    case SDLK_DELETE:
        return KeyCode::DELETE;
    case SDLK_NUMLOCKCLEAR:
        return KeyCode::NUM_LOCK;
    case SDLK_SCROLLLOCK:
        return KeyCode::SCROLL_LOCK;
    case SDLK_SEMICOLON:
        return KeyCode::SEMICOLON;
    case SDLK_EQUALS:
        return KeyCode::EQUAL;
    case SDLK_COMMA:
        return KeyCode::COMMA;
    case SDLK_MINUS:
        return KeyCode::MINUS;
    case SDLK_PERIOD:
        return KeyCode::PERIOD;
    case SDLK_SLASH:
        return KeyCode::SLASH;
    case SDLK_GRAVE:
        return KeyCode::GRAVE;
    case SDLK_LEFTBRACKET:
        return KeyCode::LEFT_BRACKET;
    case SDLK_BACKSLASH:
        return KeyCode::BACKSLASH;
    case SDLK_RIGHTBRACKET:
        return KeyCode::RIGHT_BRACKET;
    case SDLK_APOSTROPHE:
        return KeyCode::APOSTROPHE;
    case SDLK_LGUI:
        return KeyCode::LEFT_SUPER;
    case SDLK_RGUI:
        return KeyCode::RIGHT_SUPER;
    case SDLK_LALT:
        return KeyCode::LEFT_ALT;
    case SDLK_RALT:
        return KeyCode::RIGHT_ALT;
    case SDLK_KP_ENTER:
        return KeyCode::ENTER;
    case SDLK_KP_MINUS:
        return KeyCode::NUMPAD_SUBTRACT;
    case SDLK_KP_MULTIPLY:
        return KeyCode::NUMPAD_MULTIPLY;
    case SDLK_KP_PLUS:
        return KeyCode::NUMPAD_ADD;
    case SDLK_KP_DIVIDE:
        return KeyCode::NUMPAD_DIVIDE;
    case SDLK_KP_DECIMAL:
        return KeyCode::NUMPAD_DECIMAL;
    case SDLK_KP_0:
        return KeyCode::NUMPAD_0;
    }
    if(keyCode < 256)
        return (KeyCode)keyCode;
    return KeyCode::UNKNOWN;
}

int SDL3GameWindow::getKeyFromKeyCode(KeyCode code, int metaState) {
    SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;
    if(code >= KeyCode::NUM_1 && code <= KeyCode::NUM_9)
        scancode = (SDL_Scancode)((int)code - (int)KeyCode::NUM_1 + SDL_SCANCODE_1);
    else if(code >= KeyCode::NUMPAD_1 && code <= KeyCode::NUMPAD_9)
        scancode = (SDL_Scancode)((int)code - (int)KeyCode::NUMPAD_1 + SDL_SCANCODE_KP_1);
    else if(code >= KeyCode::A && code <= KeyCode::Z)
        scancode = (SDL_Scancode)((int)code - (int)KeyCode::A + SDL_SCANCODE_A);
    else if(code >= KeyCode::FN1 && code <= KeyCode::FN12)
        scancode = (SDL_Scancode)((int)code - (int)KeyCode::FN1 + SDL_SCANCODE_F1);
    else {
        switch(code) {
        case KeyCode::BACK:
            scancode = SDL_SCANCODE_AC_BACK;
            break;
        case KeyCode::BACKSPACE:
            scancode = SDL_SCANCODE_BACKSPACE;
            break;
        case KeyCode::TAB:
            scancode = SDL_SCANCODE_TAB;
            break;
        case KeyCode::ENTER:
            scancode = SDL_SCANCODE_RETURN;
            break;
        case KeyCode::LEFT_SHIFT:
            scancode = SDL_SCANCODE_LSHIFT;
            break;
        case KeyCode::RIGHT_SHIFT:
            scancode = SDL_SCANCODE_RSHIFT;
            break;
        case KeyCode::LEFT_CTRL:
            scancode = SDL_SCANCODE_LCTRL;
            break;
        case KeyCode::RIGHT_CTRL:
            scancode = SDL_SCANCODE_RCTRL;
            break;
        case KeyCode::PAUSE:
            scancode = SDL_SCANCODE_PAUSE;
            break;
        case KeyCode::CAPS_LOCK:
            scancode = SDL_SCANCODE_CAPSLOCK;
            break;
        case KeyCode::ESCAPE:
            scancode = SDL_SCANCODE_ESCAPE;
            break;
        case KeyCode::SPACE:
            scancode = SDL_SCANCODE_SPACE;
            break;
        case KeyCode::PAGE_UP:
            scancode = SDL_SCANCODE_PAGEUP;
            break;
        case KeyCode::PAGE_DOWN:
            scancode = SDL_SCANCODE_PAGEDOWN;
            break;
        case KeyCode::END:
            scancode = SDL_SCANCODE_END;
            break;
        case KeyCode::HOME:
            scancode = SDL_SCANCODE_HOME;
            break;
        case KeyCode::LEFT:
            scancode = SDL_SCANCODE_LEFT;
            break;
        case KeyCode::UP:
            scancode = SDL_SCANCODE_UP;
            break;
        case KeyCode::RIGHT:
            scancode = SDL_SCANCODE_RIGHT;
            break;
        case KeyCode::DOWN:
            scancode = SDL_SCANCODE_DOWN;
            break;
        case KeyCode::INSERT:
            scancode = SDL_SCANCODE_INSERT;
            break;
        case KeyCode::DELETE:
            scancode = SDL_SCANCODE_DELETE;
            break;
        case KeyCode::NUM_LOCK:
            scancode = SDL_SCANCODE_NUMLOCKCLEAR;
            break;
        case KeyCode::SCROLL_LOCK:
            scancode = SDL_SCANCODE_SCROLLLOCK;
            break;
        case KeyCode::SEMICOLON:
            scancode = SDL_SCANCODE_SEMICOLON;
            break;
        case KeyCode::EQUAL:
            scancode = SDL_SCANCODE_EQUALS;
            break;
        case KeyCode::COMMA:
            scancode = SDL_SCANCODE_COMMA;
            break;
        case KeyCode::MINUS:
            scancode = SDL_SCANCODE_MINUS;
            break;
        case KeyCode::NUMPAD_ADD:
            scancode = SDL_SCANCODE_KP_PLUS;
            break;
        case KeyCode::NUMPAD_SUBTRACT:
            scancode = SDL_SCANCODE_KP_MINUS;
            break;
        case KeyCode::NUMPAD_MULTIPLY:
            scancode = SDL_SCANCODE_KP_MULTIPLY;
            break;
        case KeyCode::NUMPAD_DIVIDE:
            scancode = SDL_SCANCODE_KP_DIVIDE;
            break;
        case KeyCode::PERIOD:
            scancode = SDL_SCANCODE_KP_PERIOD;
            break;
        case KeyCode::NUMPAD_DECIMAL:
            scancode = SDL_SCANCODE_KP_DECIMAL;
            break;
        case KeyCode::SLASH:
            scancode = SDL_SCANCODE_SLASH;
            break;
        case KeyCode::GRAVE:
            scancode = SDL_SCANCODE_GRAVE;
            break;
        case KeyCode::LEFT_BRACKET:
            scancode = SDL_SCANCODE_LEFTBRACKET;
            break;
        case KeyCode::BACKSLASH:
            scancode = SDL_SCANCODE_BACKSLASH;
            break;
        case KeyCode::RIGHT_BRACKET:
            scancode = SDL_SCANCODE_RIGHTBRACKET;
            break;
        case KeyCode::APOSTROPHE:
            scancode = SDL_SCANCODE_APOSTROPHE;
            break;
        case KeyCode::MENU:
            scancode = SDL_SCANCODE_MENU;
            break;
        case KeyCode::LEFT_SUPER:
            scancode = SDL_SCANCODE_LGUI;
            break;
        case KeyCode::RIGHT_SUPER:
            scancode = SDL_SCANCODE_RGUI;
            break;
        case KeyCode::LEFT_ALT:
            scancode = SDL_SCANCODE_LALT;
            break;
        case KeyCode::RIGHT_ALT:
            scancode = SDL_SCANCODE_RALT;
            break;
        case KeyCode::NUM_0:
            scancode = SDL_SCANCODE_0;
            break;
        case KeyCode::NUMPAD_0:
            scancode = SDL_SCANCODE_KP_0;
            break;
        default:
            return 0;
        }
    }
    SDL_Keymod modstate = SDL_KMOD_NONE;
    if(metaState & KEY_MOD_SHIFT) {
        modstate |= SDL_KMOD_SHIFT;
    }
    if(metaState & KEY_MOD_CTRL) {
        modstate |= SDL_KMOD_CTRL;
    }
    if(metaState & KEY_MOD_SUPER) {
        modstate |= SDL_KMOD_GUI;
    }
    if(metaState & KEY_MOD_ALT) {
        modstate |= SDL_KMOD_ALT;
    }
    if(metaState & KEY_MOD_CAPSLOCK) {
        modstate |= SDL_KMOD_CAPS;
    }
    if(metaState & KEY_MOD_NUMLOCK) {
        modstate |= SDL_KMOD_NUM;
    }
    SDL_Keycode keyCode = SDL_GetKeyFromScancode(scancode, modstate, false);
    if((modstate & SDL_KMOD_SHIFT || modstate & SDL_KMOD_CAPS) && (keyCode >= SDLK_A && keyCode <= SDLK_Z)) {
        keyCode += 'A' - 'a';
    }
    return keyCode;
}

int SDL3GameWindow::translateMeta(SDL_Keymod meta) {
    int mods = 0;
    if(meta & SDL_KMOD_SHIFT) {
        mods |= KEY_MOD_SHIFT;
    }
    if(meta & SDL_KMOD_CTRL) {
        mods |= KEY_MOD_CTRL;
    }
    if(meta & SDL_KMOD_ALT) {
        mods |= KEY_MOD_ALT;
    }
    if(meta & SDL_KMOD_GUI) {
        mods |= KEY_MOD_SUPER;
    }
    if(meta & SDL_KMOD_CAPS) {
        mods |= KEY_MOD_CAPSLOCK;
    }
    if(meta & SDL_KMOD_NUM) {
        mods |= KEY_MOD_NUMLOCK;
    }
    return mods;
}
