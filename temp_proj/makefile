# temp_proj/Makefile
CC = g++
CFLAGS = -Iinclude -Ic_api
LDFLAGS = -L. -lNetwork
TARGET = server_bin
SRC = server/server.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

run: $(TARGET)
	LD_LIBRARY_PATH=. ./$(TARGET)

clean:
	rm -f $(TARGET)
