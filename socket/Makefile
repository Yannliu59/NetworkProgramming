.PHONY:clean all
	CC=g++
	CFLAGS=-Wall -g
	BIN=epollsrv
all:$(BIN)
%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

epollsrv:epollsrv.o
	g++ $(CFLAGS) $^ -o $@

clean:
	rm -f *.o $(BIN)
.PHONY:clean all

