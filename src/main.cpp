#include "history.hpp"
#include "completion.hpp"
#include "executor.hpp"
#include <iostream>
#include <cstdlib>
#include <readline/readline.h>
#include <readline/history.h>

int main() {
    // Disable output buffering
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // Initialize history
    shell::history::init_history_file();
    using_history();
    shell::history::load_history();

    // Initialize completion
    shell::completion::init_completion();

    // Main loop
    while (true) {
        char* line = readline("$ ");
        
        if (!line) {
            // EOF (Ctrl+D)
            break;
        }
        
        if (*line) {
            add_history(line);
        }

        std::string input(line);
        free(line);

        shell::executor::execute(input);
    }

    shell::history::save_history();
    std::cout << std::endl;
    return 0;
}