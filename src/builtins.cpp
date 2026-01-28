#include "builtins.hpp"
#include "utils.hpp"
#include "history.hpp"
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <limits.h>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <readline/history.h>

namespace shell {
namespace builtins {

const std::vector<std::string> builtin_list = {
    "cd", "pwd", "echo", "exit", "type", "history"
};

bool is_builtin(const std::string& cmd) {
    return std::find(builtin_list.begin(), builtin_list.end(), cmd) 
           != builtin_list.end();
}

void builtin_pwd() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd))) {
        std::cout << cwd << std::endl;
    } else {
        std::cerr << "pwd: " << strerror(errno) << std::endl;
    }
}

void builtin_cd(const std::vector<std::string>& args) {
    std::string path;
    
    if (args.size() == 1) {
        const char* home = getenv("HOME");
        if (!home) {
            std::cerr << "cd: HOME not set\n";
            return;
        }
        path = home;
    } else {
        path = expand_tilde(args[1]);
    }

    if (chdir(path.c_str()) != 0) {
        std::cerr << "cd: " << path << ": " << strerror(errno) << std::endl;
    }
}

void builtin_echo(const std::vector<std::string>& args) {
    bool newline = true;
    size_t i = 1;
    
    if (i < args.size() && args[i] == "-n") {
        newline = false;
        ++i;
    }
    
    for (; i < args.size(); ++i) {
        std::cout << args[i];
        if (i + 1 < args.size()) {
            std::cout << " ";
        }
    }
    
    if (newline) {
        std::cout << std::endl;
    }
}

void builtin_type(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cerr << "type: missing operand\n";
        return;
    }
    
    const std::string& name = args[1];
    
    if (is_builtin(name)) {
        std::cout << name << " is a shell builtin\n";
    } else {
        std::string path = resolve_exec(name);
        if (!path.empty()) {
            std::cout << name << " is " << path << std::endl;
        } else {
            std::cout << name << ": not found\n";
        }
    }
}

void builtin_history(const std::vector<std::string>& args) {
    const char* hist_file = history::get_history_file();
    
    // Handle -c flag to clear history
    if (args.size() > 1 && args[1] == "-c") {
        clear_history();
        history::set_last_history_length(0);
        if (hist_file) {
            FILE* f = fopen(hist_file, "w");
            if (f) fclose(f);
        }
        return;
    }

    // Handle -r flag to read history from file
    if (args.size() > 2 && args[1] == "-r") {
        std::string filepath = expand_tilde(args[2]);
        if (filepath == args[2] && args[2][0] == '~') {
            std::cerr << "history: HOME not set\n";
            return;
        }
        
        if (read_history(filepath.c_str()) != 0) {
            std::cerr << "history: " << filepath << ": " 
                      << strerror(errno) << std::endl;
        }
        return;
    }

    if (args.size() == 2 && args[1] == "-r") {
        std::cerr << "history: -r: option requires an argument\n";
        return;
    }

    // Handle -w flag to write history to file
    if (args.size() > 2 && args[1] == "-w") {
        std::string filepath = expand_tilde(args[2]);
        if (filepath == args[2] && args[2][0] == '~') {
            std::cerr << "history: HOME not set\n";
            return;
        }
        
        if (write_history(filepath.c_str()) != 0) {
            std::cerr << "history: " << filepath << ": " 
                      << strerror(errno) << std::endl;
        }
        history::set_last_history_length(history_length);
        return;
    }

    if (args.size() == 2 && args[1] == "-w") {
        std::cerr << "history: -w: option requires an argument\n";
        return;
    }

    // Handle -a flag to append NEW history entries to file
    if (args.size() > 2 && args[1] == "-a") {
        std::string filepath = expand_tilde(args[2]);
        if (filepath == args[2] && args[2][0] == '~') {
            std::cerr << "history: HOME not set\n";
            return;
        }
        
        int new_entries = history_length - history::get_last_history_length();
        
        if (new_entries > 0) {
            if (append_history(new_entries, filepath.c_str()) != 0) {
                std::cerr << "history: " << filepath << ": " 
                          << strerror(errno) << std::endl;
            }
        }
        history::set_last_history_length(history_length);
        return;
    }

    if (args.size() == 2 && args[1] == "-a") {
        std::cerr << "history: -a: option requires an argument\n";
        return;
    }

    // Display history
    HIST_ENTRY** hist_list = history_list();
    if (!hist_list) return;

    int count = history_length;
    int start = 0;

    if (args.size() > 1) {
        try {
            int n = std::stoi(args[1]);
            if (n > 0 && n < count) {
                start = count - n;
            }
        } catch (...) {
            std::cerr << "history: " << args[1] 
                      << ": numeric argument required\n";
            return;
        }
    }

    for (int i = start; i < count; ++i) {
        std::cout << "  " << (i + history_base) << "  " 
                  << hist_list[i]->line << std::endl;
    }
}

int execute_builtin(const std::vector<std::string>& args) {
    if (args.empty()) return 1;
    
    const std::string& cmd = args[0];
    
    if (cmd == "pwd") {
        builtin_pwd();
    } else if (cmd == "cd") {
        builtin_cd(args);
    } else if (cmd == "echo") {
        builtin_echo(args);
    } else if (cmd == "type") {
        builtin_type(args);
    } else if (cmd == "history") {
        builtin_history(args);
    } else {
        return 1;
    }
    
    return 0;
}

} // namespace builtins
} // namespace shell