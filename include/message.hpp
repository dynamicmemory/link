#pragma once

#include <string>
#include "netevent.hpp"

struct Message {
    int fd;
    NetEvent event;
    std::string payload;
};
