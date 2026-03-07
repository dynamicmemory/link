#pragma once

class IMultiplexer {
public: 
    virtual ~IMultiplexer() = default;
    virtual void add_fd(int) = 0;
    virtual void remove_fd(int) = 0;
    virtual void wait(int) = 0;
    virtual bool ready(int) const = 0;
};
