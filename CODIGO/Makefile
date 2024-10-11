CC = gcc
CFLAGS = -I/usr/include -lcurl -ljansson
OBJ = main.o Config.o

install:
	sudo apt-get install -y libcurl4-openssl-dev libjansson-dev

all: install main

bot: $(OBJ)
	$(CC) -o main $(OBJ) $(CFLAGS)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o main
