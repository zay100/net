CC=clang++
CFLAGS=-c

all: net

net: generation_net.o net_lib.o
	$(CC) generation_net.o net_lib.o -o net

generation_net.o:  generation_net.cpp Net.h
	$(CC) $(CFLAGS) generation_net.cpp

net_lib.o:  net_lib.cpp Net.h
	$(CC) $(CFLAGS) net_lib.cpp

clean:
	rm -rf *.o net *.msh