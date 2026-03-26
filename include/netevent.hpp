/* Enum class to represent network events concerning sockets */
#pragma once

enum class NetEvent {
    NONE,
    CONNECTED,
    CLIENT_DISCONNECT,
    SERVER_DISCONNECT,
    DATA 
};
