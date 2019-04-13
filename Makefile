all: client.out server.out

client.out: client/htcpcp-client.c shared/shared.c
	gcc -Wall -g client/htcpcp-client.c -o client.o -c &&\
	gcc -Wall -g shared/shared.c -o shared.o -c &&\
	gcc -o client.out -g client.o shared.o

server.out: server/htcpcp-server.c shared/shared.c
	gcc -Wall -g server/htcpcp-server.c -o server.o -c -pthread &&\
	gcc -Wall -g shared/shared.c -o shared.o -c &&\
	gcc -o server.out -g server.o shared.o -pthread

nodotos:
	rm *.o

clean:
	rm *.o
	rm *.out
