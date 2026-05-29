CC=clang
# OPTIONS=-Wall -Wextra -Werror
OPTIONS=-Wall -Wextra

all: build

build: server client

client: client.o
	$(CC) $(LINTER_DEFINES) $(OPTIONS) $(INCLUDES) -lecal_core_c client.o -o client

client.o: client.c
	$(CC) $(LINTER_DEFINES) $(OPTIONS) $(INCLUDES) -c client.c -o client.o

server: server.o
	$(CC) $(LINTER_DEFINES) $(OPTIONS) $(INCLUDES) -lecal_core_c server.o -o server

server.o: server.c
	$(CC) $(LINTER_DEFINES) $(OPTIONS) $(INCLUDES) -c server.c -o server.o

run-client:
	./client

run-server:
	./server

clean:
	rm server client *.o
