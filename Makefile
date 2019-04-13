client.out: client/htcpcp-client.c shared/shared.c
	gcc -Wall client/htcpcp-client.c -o client.o -c
	gcc -Wall shared/shared.c -o shared.o -c
	gcc -o client.out client.o shared.o
	rm *.o

server.out: server/htcpcp-server.c shared/shared.c
	gcc -Wall server/htcpcp-server.c -o server.o -c -pthread
	gcc -Wall shared/shared.c -o shared.o -c
	gcc -o server.out server.o shared.o -pthread
	rm *.o

clean:
	rm *.o
	rm *.out
