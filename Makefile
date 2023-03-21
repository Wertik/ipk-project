CC=g++
TARGET=ipkcpc
HEADERS=arg_parser.hpp cli.hpp match_result.hpp option.hpp parse_result.hpp server_config.hpp server_mode.hpp tcp_server.hpp udp_server.hpp server.hpp
OBJS=ipkcpc.o arg_parser.o cli.o option.o parse_result.o server_config.o server_mode.o tcp_server.o udp_server.o
CFLAGS=-std=c++2a

LOGIN=xotrad00

DEF_PORT=2065
DEF_CLIENT_ARGS=-m tcp -h localhost -p $(DEF_PORT)

.PHONY: $(TARGET)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

# Run commands from file
cmds: $(TARGET)
	cat commands | ./$(TARGET) $(DEF_CLIENT_ARGS)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

# Pack for submission
pack:
	rm -rf xotrad00.zip
	zip $(LOGIN).zip *.cpp *.hpp README.md LICENSE .gitignore

clean:
	rm -rf xotrad00.zip
	rm -rf $(TARGET) $(OBJS)
