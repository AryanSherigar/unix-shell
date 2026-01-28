#  Unix-Like Shell in C++

A modular, Unix-inspired command-line shell built from scratch in **C++** using **POSIX system calls**. This project focuses on core operating system concepts such as process creation, pipes, file descriptor management, and command parsing, while also delivering a polished interactive user experience with history and tab completion.

---

##  Features

### Core Shell Functionality

* Interactive shell loop with custom prompt
* Command parsing with support for arguments and pipelines (`|`)
* Execution of external programs using `fork()`, `execvp()`, and `waitpid()`
* Input and output redirection (`<`, `>`)
* Built-in commands executed in the parent shell process

### Built-in Commands

* `cd` – change working directory
* `exit` – exit the shell
* Additional helper commands (e.g., `pwd`, `help` if implemented)

### User Experience Enhancements

* Command history using GNU Readline
* Persistent history across sessions
* Tab completion for commands
* Graceful error messages for invalid commands

---

##  Project Architecture

The shell is designed with **clear separation of concerns**, closely mirroring how real Unix shells are structured.

```
.
├── src/
│   ├── main.cpp          # Shell entry point and main loop
│   ├── parser.cpp        # Tokenization and command parsing
│   ├── executor.cpp     # Process creation, pipes, and execution
│   ├── redirection.cpp  # File descriptor and I/O redirection logic
│   ├── builtins.cpp     # Built-in shell commands
│   ├── history.cpp      # Command history handling
│   ├── completion.cpp  # Tab completion logic
│   └── utils.cpp        # Shared helper utilities
│
├── include/              # Header files for all modules
│   ├── parser.hpp
│   ├── executor.hpp
│   ├── redirection.hpp
│   ├── builtins.hpp
│   ├── history.hpp
│   ├── completion.hpp
│   └── utils.hpp
│
└── README.md
```

**Design principle:** parsing, execution, redirection, and UX are completely decoupled, making the codebase easy to extend and reason about.

---

## ⚙️ Technologies & Concepts Used

* **Language:** C++
* **System APIs:** POSIX (`fork`, `execvp`, `pipe`, `dup2`, `waitpid`, `open`)
* **Libraries:** GNU Readline
* **Core OS Concepts:**

  * Process creation and lifecycle
  * Inter-process communication (pipes)
  * File descriptor manipulation
  * Parent vs child process behavior

---

##  Building & Running

### Prerequisites

* Linux / Unix-based OS
* `g++` compiler
* GNU Readline development library

### Build

```bash
g++ -std=c++17 src/*.cpp -Iinclude -lreadline -o unix-shell
```

### Run

```bash
./unix-shell
```

---

##  Example Usage

```bash
$ ls | grep cpp > files.txt
$ cat < files.txt
$ cd /usr/bin
$ echo "Hello World"
```

---

##  Future Improvements

The following enhancements are planned to reach a fully production-grade shell:

* Signal handling (`Ctrl+C`, `Ctrl+Z`) with proper foreground process control
* Background job execution using `&`
* Job control (`jobs`, `fg`, `bg`)
* Append redirection (`>>`) and error redirection (`2>`)
* More robust handling of quoted and escaped strings
* Implement a separate UI for the shell

---



##  Author

**Aryan Keshav Sherigar**
Build during Codecrafters Unix Shell Challenge.

---

##  License

This project is open-source and available for learning and experimentation.
