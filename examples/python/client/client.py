import link

client = link.Client("127.0.0.1", "1991", "default", "tls", "select")

client.send("Hello from python brah")
while True:
    client.tick(0)
    if client.has_message():
        msg = client.next()
        print(msg.payload)
