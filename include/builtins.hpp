#ifndef BUILTINS_HPP
#define BUILTINS_HPP

#include <string>
#include <vector>

namespace shell {
namespace builtins {

/// List of all builtin command names
extern const std::vector<std::string> builtin_list;

/**
 * @brief Checks if a command is a shell builtin
 * @param cmd Command name to check
 * @return true if builtin, false otherwise
 */
bool is_builtin(const std::string& cmd);

/**
 * @brief Executes the pwd builtin command
 */
void builtin_pwd();

/**
 * @brief Executes the cd builtin command
 * @param args Command arguments (cd [path])
 */
void builtin_cd(const std::vector<std::string>& args);

/**
 * @brief Executes the echo builtin command
 * @param args Command arguments (echo [-n] [string...])
 */
void builtin_echo(const std::vector<std::string>& args);

/**
 * @brief Executes the type builtin command
 * @param args Command arguments (type name)
 */
void builtin_type(const std::vector<std::string>& args);

/**
 * @brief Executes the history builtin command
 * @param args Command arguments (history [-c|-r|-w|-a file] [n])
 */
void builtin_history(const std::vector<std::string>& args);

/**
 * @brief Executes a builtin command by name
 * @param args Command and its arguments
 * @return Exit code (0 for success)
 */
int execute_builtin(const std::vector<std::string>& args);

} // namespace builtins
} // namespace shell

#endif // BUILTINS_HPP