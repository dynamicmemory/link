#pragma once

#include <string>
#include "netevent.hpp"

/* Represents a message emitted by the networking layer.
 *
 * - fd:        Source socket descriptor.
 * - event:     Type of event associated with this message.
 * - payload:   Message data (valid only for DATA events).
 */
struct Message {
    int fd;
    NetEvent event;
    std::string payload;
};
