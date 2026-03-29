#include <errorwindow.h>
#include <EnvPathUtil.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdexcept>
#include <cstring>
#include <thread>
#include <errno.h>

std::vector<std::string> ErrorWindow::buildCommandLine(std::string title, std::string description) {
    std::vector<std::string> cmd;
    std::string path;
    if (EnvPathUtil::findInPath("mcpelauncher-error", path, ".", EnvPathUtil::getAppDir().c_str())) {
        cmd.emplace_back(path);
    } else if (EnvPathUtil::findInPath("mcpelauncher-error", path)) {
        cmd.emplace_back(path);
    } else {
        printf("Failed to find mcpelauncher-error add it into PATH to get UI errors\n");
        _exit(1);
    }
    cmd.emplace_back(title);
    cmd.emplace_back(description);
    return std::move(cmd);
}

std::vector<const char*> ErrorWindow::convertToC(std::vector<std::string> const& v) {
    std::vector<const char*> ret;
    for (auto const& i : v)
        ret.push_back(i.c_str());
    ret.push_back(nullptr);
    return std::move(ret);
}

bool ErrorWindow::onError(std::string title, std::string errormsg) {
    GameWindowErrorHandler::onError(title, errormsg);
    // Try open an errorwindow, non fatal if fails
    int pid;
    if (!(pid = fork())) {
        auto argv = buildCommandLine(title, errormsg);
        auto argvc = convertToC(argv);

        int r = execv(argvc[0], (char**) argvc.data());
        printf("Show: execv() error %i %s", r, strerror(errno));
        _exit(r);
    } else if (pid != -1) {
        std::thread([pid]() {
            int status;
            (void) waitpid(pid, &status, 0);
        }).detach();
    }
    return true;
}
