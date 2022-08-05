CC = g++
CFLAGS = -Wall -g
BIN = 01thread p_echocli p_echosrv tsd

all:$(BIN)

.PHONY:all
	%.o:%.cpp
		$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(BIN)
	
