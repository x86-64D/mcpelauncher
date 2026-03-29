#pragma once
#include <game_window_error_handler.h>
#include <vector>

class ErrorWindow : public GameWindowErrorHandler {
    std::vector<std::string> buildCommandLine(std::string title, std::string description);
    static std::vector<const char*> convertToC(std::vector<std::string> const& v);
public:
    bool onError(std::string title, std::string errormsg) override;
};