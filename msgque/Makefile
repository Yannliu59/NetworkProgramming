.PHONY:clean all
cc = g++
CFLAGS = -Wall -g
BIN = msg_get msg_stat msg_send msg_rcv msecho_cli msecho_srv
all:$(BIN)

	%.o:%.cpp:
		$(cc) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(BIN)
