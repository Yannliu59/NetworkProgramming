CC = gcc
CFLAGS = -Wall -g
BIN = shm_open shm_unlink shm_write shm_read

all:$(BIN)

.PHONY:all
	%.o:%.c
		$(CC) $(CFLAGS) -c $< -o $@

clean:
	        rm -rf *.o $(BIN)
