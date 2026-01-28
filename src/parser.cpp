#include "parser.hpp"
#include <iostream>
#include <cctype>

namespace shell {
namespace parser {

std::vector<std::string> tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::string current;

    enum class State { NORMAL, SINGLE_QUOTE, DOUBLE_QUOTE };
    State state = State::NORMAL;

    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];

        switch (state) {
        case State::NORMAL:
            if (std::isspace(c)) {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
            } else if (c == '|') {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
                tokens.push_back("|");
            } else if (c == '\'') {
                state = State::SINGLE_QUOTE;
            } else if (c == '"') {
                state = State::DOUBLE_QUOTE;
            } else if (c == '\\' && i + 1 < input.size()) {
                current += input[++i];
            } else {
                current += c;
            }
            break;

        case State::SINGLE_QUOTE:
            if (c == '\'') {
                state = State::NORMAL;
            } else {
                current += c;
            }
            break;

        case State::DOUBLE_QUOTE:
            if (c == '"') {
                state = State::NORMAL;
            } else if (c == '\\' && i + 1 < input.size()) {
                char n = input[i + 1];
                if (n == '"' || n == '\\' || n == '$' || n == '`' || n == '\n') {
                    current += n;
                    ++i;
                } else {
                    current += c;
                }
            } else {
                current += c;
            }
            break;
        }
    }

    if (state != State::NORMAL) {
        std::cerr << "shell: unmatched quote\n";
        return {};
    }

    if (!current.empty()) {
        tokens.push_back(current);
    }

    return tokens;
}

std::vector<std::vector<std::string>> split_pipeline(
        const std::vector<std::string>& tokens) {
    std::vector<std::vector<std::string>> commands;
    std::vector<std::string> current;

    for (const auto& token : tokens) {
        if (token == "|") {
            if (current.empty()) {
                return {};
            }
            commands.push_back(current);
            current.clear();
        } else {
            current.push_back(token);
        }
    }

    if (current.empty()) {
        return {};
    }
    
    commands.push_back(current);
    return commands;
}

Redirections extract_redirections(std::vector<std::string>& args) {
    Redirections redir;
    std::vector<std::string> clean;

    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& token = args[i];
        
        if ((token == ">" || token == "1>") && i + 1 < args.size()) {
            redir.stdout_file = args[++i];
        } else if ((token == ">>" || token == "1>>") && i + 1 < args.size()) {
            redir.stdout_file = args[++i];
            redir.stdout_append = true;
        } else if (token == "2>" && i + 1 < args.size()) {
            redir.stderr_file = args[++i];
        } else if (token == "2>>" && i + 1 < args.size()) {
            redir.stderr_file = args[++i];
            redir.stderr_append = true;
        } else {
            clean.push_back(token);
        }
    }

    args = std::move(clean);
    return redir;
}

} // namespace parser
} // namespace shell