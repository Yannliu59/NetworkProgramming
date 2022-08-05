cc = g++
CFLAGS = -Wall -g
BIN = shmwrite shmread
all:$(BIN)

	%.o:%.cpp:
		$(cc) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(BIN)

