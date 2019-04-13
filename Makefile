client.out: client/htcpcp-client.c shared/shared.c
	gcc -Wall client/htcpcp-client.c -o client.o -c
	gcc -Wall shared/shared.c -o shared.o -c
	gcc -o client.out client.o shared.o
	rm *.o

clean:
	rm *.o
	rm *.out
