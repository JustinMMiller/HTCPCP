# $$TODO Make this output a library instead of compiling
# directly into the executable.
all: client.out server.out

client.out: client/client.c client/htcpcp-client.c shared/shared.c
	gcc -Wall -g -o $@ $^ -pthread -Iclient

server.out: server/server.c server/htcpcp-server.c shared/shared.c
	gcc -Wall -g -o $@ $^ -pthread -Iserver

nodotos:
	rm *.o

clean:
	rm *.o
	rm *.out
