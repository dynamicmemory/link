import ctypes

# Load the shared library
lib = ctypes.CDLL("../libNetwork.so")

# Define argument and return types for safety
lib.client_create.restype = ctypes.c_void_p
lib.client_create.argtypes = [ctypes.c_char_p, ctypes.c_char_p,
                                      ctypes.c_char_p, ctypes.c_char_p,
                                      ctypes.c_char_p]

lib.client_tick.restype = None
lib.client_tick.argtypes = [ctypes.c_void_p, ctypes.c_int]

lib.client_has_message.restype = ctypes.c_int
lib.client_has_message.argtypes = [ctypes.c_void_p]

lib.client_next.restype = ctypes.c_char_p
lib.client_next.argtypes = [ctypes.c_void_p]

lib.client_send.restype = None
lib.client_send.argtypes = [ctypes.c_void_p, ctypes.c_char_p]

lib.client_destroy.restype = None
lib.client_destroy.argtypes = [ctypes.c_void_p]


# Create the client
client = lib.client_create(b"1991", b"127.0.0.1", b"default", b"tls", b"select")

# Send a message to the server
lib.client_send(client, b"Hello! This is the Python client.")

print("Starting client loop... (Ctrl+C to exit)")

try:
    while True:
        # Process incoming messages
        lib.client_tick(client, 0)  # non-blocking

        while lib.client_has_message(client):
            msg_ptr = lib.client_next(client)
            msg = ctypes.string_at(msg_ptr)
            print(f"Received: {msg.decode()}")

finally:
    lib.client_destroy(client)
