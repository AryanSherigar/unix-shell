#include "utils.hpp"
#include <cstdlib>
#include <unistd.h>
#include <sstream>

namespace shell {

std::string resolve_exec(const std::string& cmd) {
    // Handle paths with /
    if (cmd.find('/') != std::string::npos) {
        if (access(cmd.c_str(), X_OK) == 0) {
            return cmd;
        }
        return "";
    }

    // Check current directory
    std::string local = "./" + cmd;
    if (access(local.c_str(), X_OK) == 0) {
        return local;
    }

    // Search PATH
    const char* path_env = getenv("PATH");
    if (!path_env) {
        return "";
    }

    std::string dir;
    std::stringstream ss(path_env);
    while (std::getline(ss, dir, ':')) {
        std::string full = dir + "/" + cmd;
        if (access(full.c_str(), X_OK) == 0) {
            return full;
        }
    }

    return "";
}

std::string expand_tilde(const std::string& path) {
    if (path.empty() || path[0] != '~') {
        return path;
    }

    const char* home = getenv("HOME");
    if (!home) {
        return path;
    }

    return std::string(home) + path.substr(1);
}

} // namespace shell