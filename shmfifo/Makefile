cc = g++
CFLAGS = -Wall -g
BIN = shmfifo_send shmfifo_recv shmfifo_destroy

all:$(BIN)
.PHONY:all

shmfifo_send:shmfifo_send.o shmfifo.o
	g++ $^ -o $@

shmfifo_recv:shmfifo_recv.o shmfifo.o
	g++ $^ -o $@

shmfifo_destroy:shmfifo_destroy.o shmfifo.o
	g++ $^ -o $@

%.o:%.cpp
	$(cc) $(CFLAGS) -c $< -o $@


clean:
	rm -rf *.o $(BIN)
