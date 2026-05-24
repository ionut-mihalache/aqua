CC=clang
LINTER_DEFINES=-D_GNU_SOURCE
OPTIONS=-Wall -Wextra -Werror -O3
INCLUDES=-I../../../include -I../../../service/include

all: build

build: service

service: main.o
	$(CC) $(LINTER_DEFINES) $(OPTIONS) $(INCLUDES) -L/home/ubuntu/dsp-library -ldsp main.o -o service

main.o: main.c
	$(CC) $(LINTER_DEFINES) $(OPTIONS) $(INCLUDES) -c main.c -o main.o

run:
	./service

clean:
	rm service *.o
