cc = g++
CFLAGS = -Wall -g
BIN = sem semtool print dinner
all:$(BIN)

	%.o:%.cpp:
		$(cc) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(BIN)
