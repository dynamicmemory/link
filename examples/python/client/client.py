import link

client = link.Client("127.0.0.1", "1991", "default", "tls", "select")

p = 0
while True:
    client.tick(0)

    if client.is_ready() and p == 0:
        client.send("Hello from python brah")
        p += 1 

    if client.has_message():
        msg = client.next()
        print(msg.payload)
