cmake_minimum_required(VERSION 3.16)
project(shell VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wconversion
        -Wsign-conversion
    )
endif()

# Find readline library
find_library(READLINE_LIBRARY readline REQUIRED)
find_library(HISTORY_LIBRARY history)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/builtins.cpp
    src/completion.cpp
    src/executor.cpp
    src/history.cpp
    src/parser.cpp
    src/redirection.cpp
    src/utils.cpp
)

# Create executable - named "shell" to match tester expectations
add_executable(shell ${SOURCES})

# Link libraries
target_link_libraries(shell PRIVATE ${READLINE_LIBRARY})
if(HISTORY_LIBRARY)
    target_link_libraries(shell PRIVATE ${HISTORY_LIBRARY})
endif()

# Install target
install(TARGETS shell RUNTIME DESTINATION bin)