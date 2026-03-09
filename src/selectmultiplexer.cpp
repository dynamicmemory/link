#include "selectmultiplexer.hpp"
#include <stdexcept>

/* Constructs a SelectMultiplexer instance.
 *
 * Initializes internal fd_sets and sets max_fd_ to -1.
 * No file descriptors are registered at construction.
 */
SelectMultiplexer::SelectMultiplexer() {
    FD_ZERO(&master_);
    FD_ZERO(&ready_);
    max_fd_ = -1;
}

/* Registers a file descriptor to be monitored for readability.
 *
 * @param fd The file descriptor to add.
 *
 * Side effects:
 * - Updates max_fd_ if the new fd is higher than the current maximum.
 */
void SelectMultiplexer::add_fd(int fd) {
    FD_SET(fd, &master_);
    if (fd > max_fd_) max_fd_ = fd;
}

/* Removes a file descriptor from the monitored set.
 *
 * @param fd The file descriptor to remove.
 *
 * - Updates max_fd_ if the removed fd was the highest-numbered descriptor.
 * - Ensures fd is no longer polled in the next wait() call. */
void SelectMultiplexer::remove_fd(int fd) {
    FD_CLR(fd, &master_);

    if (fd == max_fd_)
        while (max_fd_ >= 0 && !FD_ISSET(max_fd_, &master_))
            max_fd_--;
}

/**
 * Waits for one or more registered file descriptors to become ready.
 * Internally calls the POSIX select() system call. After this call,
 * ready(fd) can be used to query which descriptors are ready.
 *
 * @param timeout_ms Maximum time to wait in milliseconds, 0 for non-blocking, 
 *                   -1 for blocking indefinitely.
 *
 * - Updates ready_ set to contain only the ready file descriptors. */
void SelectMultiplexer::wait(int timeout_ms) {
    ready_ = master_;
    struct timeval timeout;
    struct timeval *timeout_ptr = nullptr;

    if (timeout_ms >= 0) {
        timeout.tv_sec = timeout_ms / 1000;
        timeout.tv_usec = (timeout_ms % 1000) * 1000;
        timeout_ptr = &timeout;
    }

    if (select(max_fd_+1, &ready_, nullptr, nullptr, timeout_ptr) < 0)
        throw std::runtime_error("select failed");
}

/* Checks if a file descriptor is ready for reading.
 *
 * @param fd File descriptor to query.
 * @return true if the descriptor is ready for reading, false otherwise*/
bool SelectMultiplexer::ready(int fd) const {
    return FD_ISSET(fd, &ready_);
}

/**
 * SelectMultiplexer
 *
 * Concrete implementation of the IMultiplexer interface using
 * the POSIX select() system call.
 *
 * Responsibilities:
 * - Track a set of file descriptors to monitor for I/O readiness.
 * - Wait for one or more sockets to become readable (or writable, if extended).
 * - Provide a ready(fd) query to check which sockets have data available.
 *
 * - master_ always contains all file descriptors currently registered.
 * - ready_ is populated after each wait() call and contains only
 *   the descriptors that are ready for I/O.
 * - max_fd_ is always the highest-numbered descriptor in master_.
 *
 * - FD_SETSIZE restricts the maximum number of descriptors (≈1024).
 * - Only monitors readability in the current implementation.
 * - Designed as a default/simple multiplexer; higher performance
 *   implementations (poll/epoll/kqueue) can replace it.
 */
