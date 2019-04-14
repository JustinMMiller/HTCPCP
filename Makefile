all: client.out server.out

libs: libhtcpcpclient.a libhtcpcpserver.a

libhtcpcpclient.a: shared.o libclient.o
	ar rcs $@ $< && \
	mv $@ outputlibs/

libhtcpcpserver.a: shared.o libserver.o
	ar rcs $@ $< && \
	mv $@ outputlibs/

libserver.o: server/htcpcp-server.c
	gcc -Wall -g $^ -o libserver.o -c

libclient.o: client/htcpcp-client.c
	gcc -Wall -g $^ -o libclient.o -c

shared.o: shared/shared.c
	gcc -Wall -g $^ -o $@ -c

client.out: client/client.c client/htcpcp-client.c shared/shared.c
	gcc -Wall -g -o $@ $^ -pthread -Iclient

server.out: server/server.c server/htcpcp-server.c shared/shared.c
	gcc -Wall -g -o $@ $^ -pthread -Iserver

nodotos:
	rm *.o

clean:
	rm *.o
	rm *.out
