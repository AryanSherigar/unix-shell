#include "completion.hpp"
#include "builtins.hpp"
#include <vector>
#include <string>
#include <set>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <dirent.h>

namespace shell {
namespace completion {

char* completion_generator(const char* text, int state) {
    static std::vector<std::string> matches;
    static size_t index;

    if (state == 0) {
        matches.clear();
        index = 0;
        std::set<std::string> unique;
        std::string prefix(text);

        // Add matching builtins
        for (const auto& builtin : builtins::builtin_list) {
            if (builtin.rfind(prefix, 0) == 0) {
                unique.insert(builtin);
            }
        }

        // Add matching executables from PATH
        char* path_env = getenv("PATH");
        if (path_env) {
            std::string dir;
            std::stringstream ss(path_env);
            while (std::getline(ss, dir, ':')) {
                DIR* dp = opendir(dir.c_str());
                if (!dp) continue;
                
                dirent* ent;
                while ((ent = readdir(dp))) {
                    std::string name = ent->d_name;
                    if (name.rfind(prefix, 0) == 0) {
                        unique.insert(name);
                    }
                }
                closedir(dp);
            }
        }

        // Add matching files from current directory
        DIR* cwd = opendir(".");
        if (cwd) {
            dirent* ent;
            while ((ent = readdir(cwd))) {
                std::string name = ent->d_name;
                if (name.rfind(prefix, 0) == 0) {
                    unique.insert(name);
                }
            }
            closedir(cwd);
        }

        matches.assign(unique.begin(), unique.end());
    }

    if (index >= matches.size()) {
        return nullptr;
    }

    return strdup(matches[index++].c_str());
}

char** completion_function(const char* text, int start, int /*end*/) {
    // Only complete commands at the beginning of the line
    if (start != 0) {
        return nullptr;
    }
    
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, completion_generator);
}

void init_completion() {
    rl_attempted_completion_function = completion_function;
}

} // namespace completion
} // namespace shell