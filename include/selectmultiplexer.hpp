#pragma once

#include "imultiplexer.hpp"
#include "sys/select.h"

class SelectMultiplexer : public IMultiplexer {
private: 
    fd_set master_;
    fd_set ready_;
    int max_fd_;
public: 
    SelectMultiplexer();
    void add_fd(int) override;
    void remove_fd(int) override;
    void wait(int) override;
    bool ready(int) const override;
};
