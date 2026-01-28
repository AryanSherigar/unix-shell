#ifndef HISTORY_HPP
#define HISTORY_HPP

#include <string>

namespace shell {
namespace history {

/// Maximum number of history entries to store
constexpr int MAX_HISTORY_SIZE = 1000;

/**
 * @brief Initializes the history file path from HISTFILE env or default
 */
void init_history_file();

/**
 * @brief Loads history from file into readline history
 */
void load_history();

/**
 * @brief Saves current history to file
 */
void save_history();

/**
 * @brief Gets the path to the history file
 * @return Pointer to history file path, or nullptr if not set
 */
const char* get_history_file();

/**
 * @brief Gets the last recorded history length (for append operations)
 * @return Last history length
 */
int get_last_history_length();

/**
 * @brief Sets the last recorded history length
 * @param length New length value
 */
void set_last_history_length(int length);

} // namespace history
} // namespace shell

#endif // HISTORY_HPP