CC = gcc
CFLAGS = -Wall -g
BIN = mq_open mq_unlink mq_getattr mq_setattr mq_send mq_recv mq_notify

all:$(BIN)

.PHONY:all
	%.o:%.c
		$(CC) $(CFLAGS) -c $< -o $@

mq_open:mq_open.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt

clean:
	rm -rf *.o $(BIN)

