CC=clang
# OPTIONS=-Wall -Wextra -Werror
OPTIONS=-Wall -Wextra
EXTRA_DEFINES ?=

all: build

build: server client

client: client.o
	$(CC) $(EXTRA_DEFINES) $(LINTER_DEFINES) $(OPTIONS) $(INCLUDES) -lecal_core_c client.o -o client

client.o: client.c
	$(CC) $(EXTRA_DEFINES) $(LINTER_DEFINES) $(OPTIONS) $(INCLUDES) -c client.c -o client.o

server: server.o
	$(CC) $(EXTRA_DEFINES) $(LINTER_DEFINES) $(OPTIONS) $(INCLUDES) -lecal_core_c server.o -o server

server.o: server.c
	$(CC) $(EXTRA_DEFINES) $(LINTER_DEFINES) $(OPTIONS) $(INCLUDES) -c server.c -o server.o

run-client:
	./client $(MSG_COUNT)

run-server:
	./server

clean:
	rm server client *.o
