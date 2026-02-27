#pragma once
#include <vector>

struct Message {
    int fd;
    std::vector<char> payload;
};
