#ifndef REDIRECTION_HPP
#define REDIRECTION_HPP

#include <string>

namespace shell {
namespace redirection {

/**
 * @brief Redirects a file descriptor to a file
 * @param fd File descriptor to redirect (e.g., STDOUT_FILENO)
 * @param file Target file path
 * @param append Whether to append instead of truncate
 * @return Saved file descriptor for restoration, or -1 on error/skip
 */
int redirect_fd(int fd, const std::string& file, bool append);

/**
 * @brief Restores a file descriptor from saved state
 * @param fd File descriptor to restore
 * @param saved Saved file descriptor from redirect_fd
 */
void restore_fd(int fd, int saved);

/**
 * @brief RAII wrapper for file descriptor redirection
 */
class RedirectGuard {
public:
    RedirectGuard(int fd, const std::string& file, bool append);
    ~RedirectGuard();
    
    RedirectGuard(const RedirectGuard&) = delete;
    RedirectGuard& operator=(const RedirectGuard&) = delete;
    
    bool is_valid() const { return saved_fd_ >= 0 || file_.empty(); }

private:
    int fd_;
    int saved_fd_;
    std::string file_;
};

} // namespace redirection
} // namespace shell

#endif // REDIRECTION_HPP