CC = g++
CFLAGS = -Wall -g
BIN = 01test 02test

all:$(BIN)

.PHONY:all
	%.o:%.cpp
		$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(BIN)
