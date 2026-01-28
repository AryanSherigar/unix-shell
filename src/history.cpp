#include "history.hpp"
#include <cstdlib>
#include <string>
#include <readline/history.h>

namespace shell {
namespace history {

namespace {
    std::string history_file_path;
    const char* history_file = nullptr;
    int last_history_length = 0;
}

void init_history_file() {
    // Check HISTFILE environment variable first
    const char* histfile = getenv("HISTFILE");
    if (histfile && histfile[0] != '\0') {
        history_file_path = histfile;
        history_file = history_file_path.c_str();
        return;
    }
    
    // Fall back to default location
    const char* home = getenv("HOME");
    if (home) {
        history_file_path = std::string(home) + "/.myshell_history";
        history_file = history_file_path.c_str();
    }
}

void load_history() {
    if (history_file) {
        read_history(history_file);
        stifle_history(MAX_HISTORY_SIZE);
    }
    last_history_length = history_length;
}

void save_history() {
    if (history_file) {
        write_history(history_file);
        history_truncate_file(history_file, MAX_HISTORY_SIZE);
    }
}

const char* get_history_file() {
    return history_file;
}

int get_last_history_length() {
    return last_history_length;
}

void set_last_history_length(int length) {
    last_history_length = length;
}

} // namespace history
} // namespace shell