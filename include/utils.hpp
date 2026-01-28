#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

namespace shell {

/**
 * @brief Resolves the full path of an executable command
 * @param cmd The command name to resolve
 * @return Full path to executable, or empty string if not found
 */
std::string resolve_exec(const std::string& cmd);

/**
 * @brief Expands tilde (~) to HOME directory path
 * @param path The path to expand
 * @return Expanded path, or original if HOME not set
 */
std::string expand_tilde(const std::string& path);

} // namespace shell

#endif // UTILS_HPP