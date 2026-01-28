#include "executor.hpp"
#include "builtins.hpp"
#include "redirection.hpp"
#include "utils.hpp"
#include "history.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <vector>

namespace shell {
namespace executor {

int execute_command(const std::vector<std::string>& args,
                    const parser::Redirections& redir) {
    if (args.empty()) return 1;

    redirection::RedirectGuard stdout_guard(
        STDOUT_FILENO, redir.stdout_file, redir.stdout_append);
    redirection::RedirectGuard stderr_guard(
        STDERR_FILENO, redir.stderr_file, redir.stderr_append);

    if (builtins::is_builtin(args[0])) {
        return builtins::execute_builtin(args);
    }

    std::string exec_path = resolve_exec(args[0]);
    if (exec_path.empty()) {
        std::cerr << args[0] << ": not found\n";
        return 127;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        std::vector<char*> argv;
        for (const auto& s : args) {
            argv.push_back(const_cast<char*>(s.c_str()));
        }
        argv.push_back(nullptr);

        execv(exec_path.c_str(), argv.data());
        perror("execv");
        _exit(1);
    }

    int status;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

int execute_pipeline(std::vector<std::vector<std::string>>& pipeline,
                     const parser::Redirections& redir) {
    const size_t n = pipeline.size();
    
    if (n == 1 && builtins::is_builtin(pipeline[0][0])) {
        // Single builtin command
        redirection::RedirectGuard stdout_guard(
            STDOUT_FILENO, redir.stdout_file, redir.stdout_append);
        redirection::RedirectGuard stderr_guard(
            STDERR_FILENO, redir.stderr_file, redir.stderr_append);
        
        return builtins::execute_builtin(pipeline[0]);
    }

    // Create pipes
    // Need (n-1) pipes, each pipe has 2 file descriptors
    const size_t num_pipes = n - 1;
    std::vector<int> fds(2 * num_pipes);
    for (size_t i = 0; i < num_pipes; ++i) {
        pipe(&fds[i * 2]);
    }

    // Fork processes
    for (size_t i = 0; i < n; ++i) {
        pid_t pid = fork();
        
        if (pid == 0) {
            // Child process
            
            // Set up input from previous pipe
            if (i > 0) {
                dup2(fds[(i - 1) * 2], STDIN_FILENO);
            }
            
            // Set up output to next pipe
            if (i < n - 1) {
                dup2(fds[i * 2 + 1], STDOUT_FILENO);
            }
            
            // Apply redirections to last command
            if (i == n - 1) {
                redirection::redirect_fd(
                    STDOUT_FILENO, redir.stdout_file, redir.stdout_append);
                redirection::redirect_fd(
                    STDERR_FILENO, redir.stderr_file, redir.stderr_append);
            }
            
            // Close all pipe fds
            for (int fd : fds) {
                close(fd);
            }

            auto& cmd = pipeline[i];
            
            if (builtins::is_builtin(cmd[0])) {
                builtins::execute_builtin(cmd);
                _exit(0);
            }

            std::string exec_path = resolve_exec(cmd[0]);
            if (exec_path.empty()) {
                std::cerr << cmd[0] << ": not found\n";
                _exit(127);
            }

            std::vector<char*> argv;
            for (auto& s : cmd) {
                argv.push_back(const_cast<char*>(s.c_str()));
            }
            argv.push_back(nullptr);

            execv(exec_path.c_str(), argv.data());
            perror("execv");
            _exit(1);
        }
    }

    // Parent: close all pipes and wait
    for (int fd : fds) {
        close(fd);
    }
    
    for (size_t i = 0; i < n; ++i) {
        wait(nullptr);
    }

    return 0;
}

bool execute(const std::string& input) {
    auto tokens = parser::tokenize(input);
    if (tokens.empty()) return true;

    auto pipeline = parser::split_pipeline(tokens);
    if (pipeline.empty()) return true;

    // Handle exit specially
    if (pipeline.size() == 1 && pipeline[0][0] == "exit") {
        auto& args = pipeline[0];
        int code = 0;
        if (args.size() > 1) {
            try {
                code = std::stoi(args[1]);
            } catch (...) {
                std::cerr << "exit: numeric argument required\n";
                code = 1;
            }
        }
        history::save_history();
        exit(code);
    }

    // Extract redirections from last command
    auto redir = parser::extract_redirections(pipeline.back());

    execute_pipeline(pipeline, redir);
    return true;
}

} // namespace executor
} // namespace shell