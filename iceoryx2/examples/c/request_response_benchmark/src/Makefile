CC=clang
# OPTIONS=-Wall -Wextra -Werror
OPTIONS=-Wall -Wextra
EXTRA_DEFINES ?=
INCLUDES=-I/home/ubuntu/dsp-library/iceoryx2/target/ffi/build/rust/native/release/iceoryx2-ffi-c-cbindgen/include

ICEORYX_DIR=/home/ubuntu/dsp-library/iceoryx2/target/ffi/build/rust/native/release
LIBS=$(ICEORYX_DIR)/libiceoryx2_ffi_c.a
# LIBS=$(ICEORYX_DIR)

all: build

build: server client

client: client.o
	$(CC) $(EXTRA_DEFINES) $(LINTER_DEFINES) $(OPTIONS) $(INCLUDES) client.o $(LIBS) -o client

client.o: client.c transmission_data.h
	$(CC) $(EXTRA_DEFINES) $(LINTER_DEFINES) $(OPTIONS) $(INCLUDES) -c client.c -o client.o

server: server.o
	$(CC) $(EXTRA_DEFINES) $(LINTER_DEFINES) $(OPTIONS) $(INCLUDES) server.o $(LIBS) -o server

server.o: server.c transmission_data.h
	$(CC) $(EXTRA_DEFINES) $(LINTER_DEFINES) $(OPTIONS) $(INCLUDES) -c server.c -o server.o

run-client:
	./client $(MSG_COUNT)

run-server:
	./server

clean:
	rm server client *.o
