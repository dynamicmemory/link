# Link

**Link** is a configurable message-oriented networking library written in C++.

It provides a modular networking stack where the core components of a connection can be assembled independently:
- **Protocol** - How messages are framed and decoded
- **Transport** - Add security with TLS or keep it simple with plain TCP
- **Multiplexer** - How sockets are monitored for I/O

Note: Currently only a fixed length pre-fixed protocol is support, and 'select' multiplexing. There are plans to add in other options.

The project is native in C++ but also exposes a C API for cross-language integration and use as well as 
python specific bindings via pybind11

---

# Design

Link is built around a layered networking model:

```
Application
    │
Protocol
    │
Transport
    │
Socket
    │
Multiplexer
    │
Operating System
```

Each layer is responsible for a single concern.

| Layer | Responsibility |
|------|---------------|
| Protocol | converts byte streams into messages |
| Transport | performs encrypted or plaintext network I/O |
| Socket | manages the lifetime of OS file descriptors |
| Multiplexer | waits for sockets to become ready for I/O |

Because each layer is isolated, new implementations can be introduced without rewriting the rest of the stack.

---

# Features

Current capabilities include:

- message-oriented networking
- configurable protocol layer
- configurable transport layer
- configurable I/O multiplexing
- TLS support via OpenSSL
- C++ / C / Python APIs

---

# Current Implementations

## Protocol

```
DefaultProtocol
```

A simple **length-prefixed framing protocol**.

Frame format:

```
[4 byte length prefix][payload]
```

The protocol buffers incoming data and extracts messages as soon as a full frame is available.

---

## Transport

```
TCPTransport
TLSTransport
```

### TCPTransport

- standard TCP communication

### TLSTransport

- OpenSSL-based TLS transport
- supports non-blocking handshake

---

## Multiplexer

```
SelectMultiplexer
```

A multiplexer built on the POSIX `select()` system call that waits for sockets to become ready for reading.

This provides a simple event-driven model without requiring threads.

Future multiplexers may include:

```
epoll
kqueue
poll
```

---

# Event Loop Model

Applications drive the network stack through a `tick()` loop.

`tick()` performs:

1. waiting for socket readiness
2. reading incoming data
3. decoding messages
4. placing messages into an application queue

Example pattern:

```cpp
while (true) {
    server.tick();

    if (server.has_message()) {
        auto msg = server.next();
        // handle message
    }
}
```

---

# Example: Server

```cpp
Server server("127.0.0.1", "1991", "default", "tls", "select");

while (true) {
    server.tick();

    if (server.has_message()) {
        auto message = server.next();

        std::cout << message.payload << std::endl;

        server.send(message.fd, "message received");
    }
}
```

---

# Example: Client

```cpp
Client client("127.0.0.1", "1991", "default", "tls", "select");

client.send("hello server");

while (true) {
    client.tick();

    if (client.has_message()) {
        auto message = client.next();
        std::cout << message.payload << std::endl;
    }
}
```

---

# Python Usage

Python bindings are provided through **pybind11**.

```python
import link

client = link.Client("127.0.0.1", "1991", "default", "tls", "select")

client.send("hello")

while True:
    client.tick()

    if client.has_message():
        msg = client.next()
        print(msg.payload)
```

---

# C API

The C API allows integration with other languages or environments.

Example:

```c
NetworkClient *client =
    client_create("127.0.0.1","1991","default","tcp","select");

client_send(client, "hello");

while (1) {
    client_tick(client, 0);

    if (client_has_message(client)) {
        const char *msg = client_next(client);
        printf("%s\n", msg);
    }
}
```

---

# Project Direction

Link is currently functional but still evolving.

Future work may include:

- additional multiplexers (`epoll`, `kqueue`)
- additional transports
- improved TLS configuration
- configurable certificate paths
- more protocol implementations
- performance improvements based on real application usage

The library will evolve as it is used in real applications and design weaknesses become visible.

---

# Dependencies

- POSIX sockets
- OpenSSL
- pybind11 (optional)

---

# License

To KILL
