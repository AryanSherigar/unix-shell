# C-shell

`C-shell` is a robust, Unix-inspired command-line interpreter built entirely from scratch in C++. It leverages core POSIX system calls to deliver a fully functional, interactive terminal experience complete with process pipelining, I/O redirection, and persistent history.

## Motivation

For a long time, using the terminal felt like pure magic to me. I would type a command, press enter, and things just happened seamlessly. I built `C-shell` because I wanted to pull back the curtain and demystify that process. I wanted to understand exactly how Linux creates processes, manages file descriptors, and actually executes commands under the hood. What started as an effort to decode that "black magic" turned into a fully capable tool that handles complex process orchestration and system-level interactions.

## Quick Start

### 1. Install Dependencies
You'll need a C++ compiler and the GNU Readline library.

**Ubuntu / Debian:**
```bash
sudo apt-get update
sudo apt-get install g++ libreadline-dev
```

**macOS:**
```bash
brew install readline
```

### 2. Build and Run
```bash
# Clone the repository
git clone [https://github.com/aryansherigar/unix-shell.git](https://github.com/aryansherigar/unix-shell.git)
cd unix-shell

# Compile the source code
g++ -std=c++17 src/*.cpp -Iinclude -lreadline -o c-shell

# Start the shell
./c-shell
```

## 📖 Usage & Capabilities

`C-shell` doesn't just parse text; it manages full process lifecycles using `fork()`, `execv()`, `waitpid()`, and `pipe()`. 

### Core Execution & Pipelining
Run standard Linux executables and chain them together. The shell automatically handles the inter-process communication (IPC) required to route data between programs.
```bash
$ ls -la | grep ".cpp" | wc -l
```

### Advanced I/O Redirection
Control standard output and standard error streams natively, just like a standard Unix shell.
```bash
# Overwrite output
$ echo "Hello" > output.txt

# Append output
$ echo "World" >> output.txt

# Redirect standard error
$ cat non_existent_file 2> error_log.txt

# Append standard error
$ ./failing_script 2>> error_log.txt
```

### Built-in Commands
Commands that modify the state of the shell itself must be executed in the parent process. `C-shell` includes native implementations for:
* `cd <path>` / `cd ~` : Navigate the file system.
* `pwd` : Print the current working directory.
* `echo [-n] <text>` : Print text to the terminal.
* `type <command>` : Identify if a command is a built-in or an external executable.
* `history [-c|-r|-w|-a]` : View and manage your command history.
* `exit <code>` : Gracefully terminate the shell.

### Interactive Enhancements
* **Command History:** Powered by GNU Readline. Use the Up/Down arrows to navigate previous commands. History is saved persistently across sessions.
* **Tab Completion:** Hit `TAB` to auto-complete built-in commands, external executables found in your `$PATH`, or files in your current directory.
* **Quote Handling:** Intelligently parses both single (`'`) and double (`"`) quotes, including escape characters (`\`).

## Project Architecture

The codebase is engineered with a strict separation of concerns, making the shell highly modular and easy to extend:

* **Parser (`parser.cpp`)**: Tokenizes raw input strings, manages quote states, and splits commands into distinct pipeline execution blocks.
* **Executor (`executor.cpp`)**: The heart of the shell. Manages process forking, sets up file descriptors for pipes, and triggers the `execv` calls.
* **Redirection (`redirection.cpp`)**: Uses an RAII pattern (`RedirectGuard`) to safely duplicate (`dup2`), manipulate, and restore file descriptors.
* **Builtins (`builtins.cpp`)**: Logic for all native commands.
* **UX Modules (`completion.cpp`, `history.cpp`)**: Interfaces with the external Readline library for a polished interactive experience.

---
*Built by Aryan Keshav Sherigar*
