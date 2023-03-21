CC=g++
TARGET=ipkcpc
CFLAGS=-std=c++2a

DEF_PORT=2065
DEF_CLIENT_ARGS=-m tcp -h localhost -p $(DEF_PORT)

.PHONY: $(TARGET)

all: $(TARGET) server

$(TARGET): $(TARGET).o

# Run commands from file
cmds: $(TARGET)
	cat commands | ./$(TARGET) $(DEF_CLIENT_ARGS)

# run echo server
rs: server
	./server $(DEF_PORT)

# Simple echo server coppied from stubs
server: server.o

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(TARGET) $(TARGET).o
