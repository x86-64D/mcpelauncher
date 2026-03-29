#include "window_manager_sdl3.h"
#include "window_sdl3.h"
#include <stdexcept>
#include <fstream>

#include <SDL3/SDL.h>

SDL3WindowManager::SDL3WindowManager() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD);
}

GameWindowManager::ProcAddrFunc SDL3WindowManager::getProcAddrFunc() {
    return (GameWindowManager::ProcAddrFunc) SDL_GL_GetProcAddress;
}

std::shared_ptr<GameWindow> SDL3WindowManager::createWindow(const std::string& title, int width, int height,
                                                             GraphicsApi api) {
    return std::shared_ptr<GameWindow>(new SDL3GameWindow(title, width, height, api));
}

void SDL3WindowManager::addGamepadMappingFile(const std::string &path) {
    // Not using this due to regression with the Gamepad Tool and ignoring all mappings without platform tag followed by comma
    // SDL_AddGamepadMappingsFromFile(path.data());
    std::string plat = SDL_GetPlatform();
    std::string platsearchpattern1 = ",platform:" + plat;
    std::string platsearchpattern2 = platsearchpattern1 + ",";
    std::ifstream fs(path);
    if (!fs)
        return;
    std::string line;
    int gamepads = 0;
    while (std::getline(fs, line)) {
        if (!line.empty() && (line.find(",platform:") == std::string::npos || line.find(platsearchpattern2) != std::string::npos || line.find(platsearchpattern1, line.size() - platsearchpattern1.size()) != std::string::npos)) {
            if(SDL_AddGamepadMapping(line.data()) >= 0) {
                gamepads++;
            }
        }
    }
}

void SDL3WindowManager::addGamePadMapping(const std::string &content) {
    SDL_AddGamepadMapping(content.data());
}

// Define this window manager as the used one
std::shared_ptr<GameWindowManager> GameWindowManager::createManager() {
    return std::shared_ptr<GameWindowManager>(new SDL3WindowManager());
}