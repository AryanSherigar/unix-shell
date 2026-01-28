#include "redirection.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>

namespace shell {
namespace redirection {

int redirect_fd(int fd, const std::string& file, bool append) {
    if (file.empty()) {
        return -1;
    }
    
    int saved = dup(fd);
    int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
    int newfd = open(file.c_str(), flags, 0644);
    
    if (newfd < 0) {
        perror("open");
        close(saved);
        return -1;
    }
    
    dup2(newfd, fd);
    close(newfd);
    return saved;
}

void restore_fd(int fd, int saved) {
    if (saved >= 0) {
        dup2(saved, fd);
        close(saved);
    }
}

RedirectGuard::RedirectGuard(int fd, const std::string& file, bool append)
    : fd_(fd), saved_fd_(-1), file_(file) {
    if (!file.empty()) {
        saved_fd_ = redirect_fd(fd, file, append);
    }
}

RedirectGuard::~RedirectGuard() {
    restore_fd(fd_, saved_fd_);
}

} // namespace redirection
} // namespace shell