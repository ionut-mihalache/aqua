CC=clang
LINTER_DEFINES=-D_GNU_SOURCE
OPTIONS=-Wall -Wextra -Werror -O3
INCLUDES=-I../../../include -I../../../client/include

all: build

build: client

client: main.o
	$(CC) $(LINTER_DEFINES) $(OPTIONS) $(INCLUDES) -L/home/ubuntu/dsp-library -ldsp main.o -o client

main.o: main.c
	$(CC) $(LINTER_DEFINES) $(OPTIONS) $(INCLUDES) -c main.c -o main.o

run:
	./client

clean:
	rm client *.o
