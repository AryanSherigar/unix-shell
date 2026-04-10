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

// Complete list of shell builtins handled internally without forking a process.
// Must be kept in sync with execute_builtin() dispatch logic.
const std::vector<std::string> builtin_list = {
    "cd", "pwd", "echo", "exit", "type", "history"
};

/**
 * @brief Checks whether the given command name is a shell builtin.
 *
 * @param cmd The command name to look up.
 * @return true if the command is a builtin, false otherwise.
 */
bool is_builtin(const std::string& cmd) {
    return std::find(builtin_list.begin(), builtin_list.end(), cmd)
           != builtin_list.end();
}

/**
 * @brief Prints the shell's current working directory to stdout.
 *
 * Uses getcwd() rather than $PWD to reflect the true filesystem path,
 * avoiding stale values after symlink traversal.
 */
void builtin_pwd() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd))) {
        std::cout << cwd << std::endl;
    } else {
        std::cerr << "pwd: " << strerror(errno) << std::endl;
    }
}

/**
 * @brief Changes the shell's working directory.
 *
 * With no argument, navigates to $HOME, mirroring POSIX cd behaviour.
 * Tilde expansion is applied before the chdir() call.
 *
 * @param args Tokenised command line; args[0] == "cd".
 */
void builtin_cd(const std::vector<std::string>& args) {
    std::string path;

    if (args.size() == 1) {
        // No target supplied — fall back to the user's home directory.
        const char* home = getenv("HOME");
        if (!home) {
            std::cerr << "cd: HOME not set\n";
            return;
        }
        path = home;
    } else {
        // Expand leading '~' before handing the path to the OS.
        path = expand_tilde(args[1]);
    }

    if (chdir(path.c_str()) != 0) {
        std::cerr << "cd: " << path << ": " << strerror(errno) << std::endl;
    }
}

/**
 * @brief Writes its arguments to stdout, optionally suppressing the newline.
 *
 * Supports the -n flag (suppress trailing newline) as the only option,
 * consistent with common shell implementations.
 *
 * @param args Tokenised command line; args[0] == "echo".
 */
void builtin_echo(const std::vector<std::string>& args) {
    bool newline = true;
    size_t i = 1;

    // Consume -n before iterating over the remaining words.
    if (i < args.size() && args[i] == "-n") {
        newline = false;
        ++i;
    }

    for (; i < args.size(); ++i) {
        std::cout << args[i];
        // Separate words with a single space, but not after the last word.
        if (i + 1 < args.size()) {
            std::cout << " ";
        }
    }

    if (newline) {
        std::cout << std::endl;
    }
}

/**
 * @brief Reports whether a name is a builtin, alias, or external executable.
 *
 * Mirrors the behaviour of the POSIX 'type' utility. Builtins are checked
 * first; external resolution falls back to PATH lookup via resolve_exec().
 *
 * @param args Tokenised command line; args[0] == "type".
 */
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

/**
 * @brief Implements the 'history' builtin with bash-compatible flags.
 *
 * Supported flags:
 *   -c          Clear the in-memory history list and truncate the history file.
 *   -r <file>   Replace the current history list with entries from <file>.
 *   -w <file>   Overwrite <file> with the entire current history list.
 *   -a <file>   Append only the new entries (added since the last -a or -w)
 *               to <file>, avoiding duplication across sessions.
 *   <n>         Display the n most recent history entries.
 *   (none)      Display the full history list.
 *
 * @param args Tokenised command line; args[0] == "history".
 */
void builtin_history(const std::vector<std::string>& args) {
    const char* hist_file = history::get_history_file();

    // -c: wipe both the in-memory list and the on-disk file so that
    //     a subsequent save does not resurrect old entries.
    if (args.size() > 1 && args[1] == "-c") {
        clear_history();
        history::set_last_history_length(0);
        if (hist_file) {
            // Truncate the file rather than deleting it to preserve permissions.
            FILE* f = fopen(hist_file, "w");
            if (f) fclose(f);
        }
        return;
    }

    // -r <file>: load (merge) history from an explicit file path.
    if (args.size() > 2 && args[1] == "-r") {
        std::string filepath = expand_tilde(args[2]);

        // expand_tilde returns the original string when HOME is unset,
        // so detect that case to avoid a misleading errno message.
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

    // Guard against -r being passed without a filename argument.
    if (args.size() == 2 && args[1] == "-r") {
        std::cerr << "history: -r: option requires an argument\n";
        return;
    }

    // -w <file>: overwrite the target file with the full history list.
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
        // Record the baseline so that a later -a only appends truly new entries.
        history::set_last_history_length(history_length);
        return;
    }

    if (args.size() == 2 && args[1] == "-w") {
        std::cerr << "history: -w: option requires an argument\n";
        return;
    }

    // -a <file>: append only the entries added in this session, preventing
    //            duplicate lines when multiple shell instances share one file.
    if (args.size() > 2 && args[1] == "-a") {
        std::string filepath = expand_tilde(args[2]);
        if (filepath == args[2] && args[2][0] == '~') {
            std::cerr << "history: HOME not set\n";
            return;
        }

        // Calculate how many new entries have accumulated since the last save.
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

    // --- Display history ---

    HIST_ENTRY** hist_list = history_list();
    if (!hist_list) return;  // Nothing to display if the list is empty.

    int count = history_length;
    int start = 0;  // Default: show the entire list.

    if (args.size() > 1) {
        try {
            int n = std::stoi(args[1]);
            // Only trim the list when n is a valid positive count smaller
            // than the total; otherwise show everything.
            if (n > 0 && n < count) {
                start = count - n;
            }
        } catch (...) {
            std::cerr << "history: " << args[1]
                      << ": numeric argument required\n";
            return;
        }
    }

    // history_base offsets line numbers to match the readline numbering scheme,
    // which allows '!n' expansion to reference the correct entry.
    for (int i = start; i < count; ++i) {
        std::cout << "  " << (i + history_base) << "  "
                  << hist_list[i]->line << std::endl;
    }
}

/**
 * @brief Dispatches a parsed command to its builtin implementation.
 *
 * This is the single entry point called by the main execution loop when
 * is_builtin() returns true. Returns 0 on success, 1 if the command is
 * not recognised as a builtin (should not normally occur).
 *
 * @param args Tokenised command line; args[0] is the command name.
 * @return Exit-status integer (0 == success).
 */
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
        return 1;  // Caller should not reach here if is_builtin() was checked.
    }

    return 0;
}

} // namespace builtins
} // namespace shell
