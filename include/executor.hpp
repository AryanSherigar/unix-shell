#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include <string>
#include <vector>
#include "parser.hpp"

namespace shell {
namespace executor {

/**
 * @brief Executes a single command (handles both builtins and external)
 * @param args Command and arguments
 * @param redirections Redirection settings
 * @return Exit code
 */
int execute_command(const std::vector<std::string>& args,
                    const parser::Redirections& redirections);

/**
 * @brief Executes a pipeline of commands
 * @param pipeline Vector of commands to execute in pipeline
 * @param redirections Redirections for the last command
 * @return Exit code of last command
 */
int execute_pipeline(std::vector<std::vector<std::string>>& pipeline,
                     const parser::Redirections& redirections);

/**
 * @brief Main execution entry point
 * @param input Raw input line
 * @return true to continue shell, false to exit
 */
bool execute(const std::string& input);

} // namespace executor
} // namespace shell

#endif // EXECUTOR_HPP