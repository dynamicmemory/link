#include "selectmultiplexer.hpp"
#include <stdexcept>

/**/ 
SelectMultiplexer::SelectMultiplexer() {
    FD_ZERO(&master_);
    FD_ZERO(&ready_);
    max_fd_ = -1;
}

/**/
void SelectMultiplexer::add_fd(int fd) {
    FD_SET(fd, &master_);
    if (fd > max_fd_) max_fd_ = fd;
}

/**/
void SelectMultiplexer::remove_fd(int fd) {
    FD_CLR(fd, &master_);

    if (fd == max_fd_)
        while (max_fd_ >= 0 && !FD_ISSET(max_fd_, &master_))
            max_fd_--;
}

/**/
void SelectMultiplexer::wait(int timeout_ms) {
    ready_ = master_;
    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    if (select(max_fd_+1, &ready_, 0, 0, &timeout) < 0)
        throw std::runtime_error("select failed");
}

/**/
bool SelectMultiplexer::ready(int fd) const {
    return FD_ISSET(fd, &ready_);
}
