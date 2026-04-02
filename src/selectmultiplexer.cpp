/**
 * SelectMultiplexer concrete implementation of the IMultiplexer interface using
 * the POSIX select() system call.
 * - Track a set of file descriptors to monitor for I/O readiness.
 * - Wait for one or more sockets to become readable (or writable, if extended).
 * - Provide a ready(fd) query to check which sockets have data available.
 *
 * - master_ always contains all file descriptors currently registered.
 * - ready_ is populated after each wait() call and contains only
 *   the descriptors that are ready for I/O.
 * - max_fd_ is always the highest-numbered descriptor in master_.
 *
 * - FD_SETSIZE restricts the maximum number of descriptors (1024).
 * - Only monitors readability in the current implementation. */

#include "selectmultiplexer.hpp"
#include <stdexcept>

/* Constructs a SelectMultiplexer instance initialising internal fd_sets and 
 * sets max_fd_ to -1. */
SelectMultiplexer::SelectMultiplexer() {
    FD_ZERO(&master_);
    FD_ZERO(&ready_);
    max_fd_ = -1;
}

/* Registers the passed in fd to be monitored for readability. */
void SelectMultiplexer::add_fd(int fd) {
    FD_SET(fd, &master_);
    if (fd > max_fd_) max_fd_ = fd;
}

/* Removes the passed in fd from the monitored set. Updates max_df_ if the 
 * removed socket was the hightest numbered fd. */
void SelectMultiplexer::remove_fd(int fd) {
    FD_CLR(fd, &master_);

    if (fd == max_fd_)
        while (max_fd_ >= 0 && !FD_ISSET(max_fd_, &master_))
            max_fd_--;
}

/* Waits for one or more registered fds to become ready. Internally calls the 
 * POSIX select() system call. After this call, ready(fd) can be used to query 
 * which fds are ready. 
 * @param timeout_ms -1 for blocking select call, 0 for non-blocking, > 0 for 
                     specific timeout length */
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

/* Returns true if passed in socket has been set to read, false otherwise */
bool SelectMultiplexer::ready(int fd) const {
    return FD_ISSET(fd, &ready_);
}

