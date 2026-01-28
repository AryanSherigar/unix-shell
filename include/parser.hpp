#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

namespace shell {
namespace parser {

/**
 * @brief Tokenizes input string handling quotes and escapes
 * @param input Raw input string
 * @return Vector of tokens, empty if parse error
 */
std::vector<std::string> tokenize(const std::string& input);

/**
 * @brief Splits tokens into pipeline commands
 * @param tokens Tokenized input
 * @return Vector of commands (each command is a vector of arguments)
 */
std::vector<std::vector<std::string>> split_pipeline(const std::vector<std::string>& tokens);

/**
 * @brief Structure to hold redirection information
 */
struct Redirections {
    std::string stdout_file;
    std::string stderr_file;
    bool stdout_append = false;
    bool stderr_append = false;
};

/**
 * @brief Extracts redirections from command arguments
 * @param args Command arguments (modified to remove redirection tokens)
 * @return Redirection information
 */
Redirections extract_redirections(std::vector<std::string>& args);

} // namespace parser
} // namespace shell

#endif // PARSER_HPP