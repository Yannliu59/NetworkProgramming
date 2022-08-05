CC = g++
CFLAGS = -g -Wall
all:threadtest
threadtest:threadtest.o threadpool.o condition.o
	$(CC) $(CFLAGS) $^ -o $@
threadtest.o:threadtest.cpp threadpool.h
	$(CC) $(CFLAGS) -c $< -o $@
threadpool:threadpool.o condition.o
	$(CC) $(CFLAGS) $^ -o $@
threadpool.o:threadpool.cpp condition.h
	$(CC) $(CFLAGS) -c $< -o $@
condition.o:condition.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o
