import ctypes

# Load the shared library
lib = ctypes.CDLL("../libNetwork.so")

# Define argument and return types for safety
lib.network_client_create.restype = ctypes.c_void_p
lib.network_client_create.argtypes = [ctypes.c_char_p, ctypes.c_char_p,
                                      ctypes.c_char_p, ctypes.c_char_p,
                                      ctypes.c_char_p]

lib.network_client_tick.restype = None
lib.network_client_tick.argtypes = [ctypes.c_void_p, ctypes.c_int]

lib.network_client_has_message.restype = ctypes.c_int
lib.network_client_has_message.argtypes = [ctypes.c_void_p]

lib.network_client_next.restype = ctypes.c_char_p
lib.network_client_next.argtypes = [ctypes.c_void_p]

lib.network_client_send.restype = None
lib.network_client_send.argtypes = [ctypes.c_void_p, ctypes.c_char_p]

lib.network_client_destroy.restype = None
lib.network_client_destroy.argtypes = [ctypes.c_void_p]


# Create the client
client = lib.network_client_create(b"1991", b"127.0.0.1", b"default", b"tls", b"select")

# Send a message to the server
lib.network_client_send(client, b"Hello! This is the Python client.")

print("Starting client loop... (Ctrl+C to exit)")

try:
    while True:
        # Process incoming messages
        lib.network_client_tick(client, 0)  # non-blocking

        while lib.network_client_has_message(client):
            msg_ptr = lib.network_client_next(client)
            msg = ctypes.string_at(msg_ptr)
            print(f"Received: {msg.decode()}")

finally:
    lib.network_client_destroy(client)
