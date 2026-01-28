#ifndef COMPLETION_HPP
#define COMPLETION_HPP

#include <readline/readline.h>

namespace shell {
namespace completion {

/**
 * @brief Generator function for readline completion
 * @param text Text to complete
 * @param state State (0 for first call)
 * @return Allocated string with match, or nullptr when done
 */
char* completion_generator(const char* text, int state);

/**
 * @brief Completion function for readline
 * @param text Text being completed
 * @param start Start position in line
 * @param end End position in line (unused)
 * @return Array of matches, or nullptr
 */
char** completion_function(const char* text, int start, int end);

/**
 * @brief Initializes readline completion
 */
void init_completion();

} // namespace completion
} // namespace shell

#endif // COMPLETION_HPP