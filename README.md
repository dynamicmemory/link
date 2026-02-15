# C++ Modular Network Framework & Infrustucture (Working title)

---

## Mapping & Ideas 

User external to library calls...

#include <Network.h>

```python
from network import Network 

Network net 

server = net.create_server(port, protocol)
client = net.create_client(host, port, protocol)

server.register_client(client)

server.send()
client.senc()
server.recieve()
client.recieve()

client.disconnect()
server.shutdown()

```

etc...

Network: 
- sockets 
- connection
- transportation
- protocols


Framework provides:
- A way to start a server 
- A way to start a client 
- A way to send and recieve data 
- A way to plug in a protocol 
- A way to shutdown with clean state 

It can: 
- handle multiple servers and clients 
- Mixed protocols

V1:
One server per network, many clients, async (multiplex select style), no blocking

V2: 
Multi server per network, many clients, async (multiplex select style), no blocking.

Class structure:

TCPsocket 
Connection 
Listener 
Transport 
Server 
Client 
Protocol 
Network 

