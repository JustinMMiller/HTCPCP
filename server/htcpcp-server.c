#include "htcpcp-server.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <regex.h>
#include <unistd.h>
#include <stdio.h>


#define MAX_THREADS_HTCPCP_SERVER 32
#define PORT 489
#define BUFFER_SIZE 4096

HTCPCPServer *getServer()
{
	HTCPCPServer *server = malloc(sizeof(HTCPCPServer));
	server->callbacks = calloc(50, sizeof(callbackFunc*));
	server->callbackMethods = calloc(50, sizeof(int));
	server->callbackURLs = calloc(50, sizeof(char*));
	pthread_mutex_init(&(server->lock), NULL);
	server->numCallbacks = 0;
	server->maxCallbacks = 50;
	return server;
}

void addRoute(HTCPCPServer *server, int method, char *url, callbackFunc *callback)
{
	pthread_mutex_lock(&(server->lock));
	int n = server->numCallbacks;
	if(server->numCallbacks < server->maxCallbacks)
	{
		server->callbacks[n] = callback;
		server->callbackMethods[n] = method;
		char *str = malloc(strlen(url) + 1);
		strcpy(str, url);
		server->callbackURLs[n] = str;
		server->numCallbacks++;
		pthread_mutex_unlock(&(server->lock));
		return;
	}
	else
	{
		int *tmethods = malloc(sizeof(int)*(n + (n/2)));
		char **turls = malloc(sizeof(char*)*(n + (n/2)));
		callbackFunc **tfuncs = malloc(sizeof(callbackFunc*)*(n+(n/2)));
		memcpy(tmethods, server->callbackMethods, n*sizeof(int));
		memcpy(turls, server->callbackURLs, n*sizeof(char*));
		memcpy(tfuncs, server->callbacks, n*sizeof(callbackFunc*));
		free(server->callbackMethods);
		free(server->callbacks);
		free(server->callbackURLs);
		server->callbacks = tfuncs;
		server->callbackMethods = tmethods;
		server->callbackURLs = turls;
		server->maxCallbacks = (n*(n/2));
		pthread_mutex_unlock(&(server->lock));
		addRoute(server, method, url, callback);
	}
}

int getCallbackIndex(HTCPCPServer *server, int method, char *callbackURL)
{
	pthread_mutex_lock(&(server->lock));
	for(int i = 0; i < server->numCallbacks; i++)
	{
		if(server->callbackMethods[i] == method &&
				!strcmp(callbackURL, server->callbackURLs[i]))
		{
			pthread_mutex_unlock(&(server->lock));
			return i;
		}
	}
	return -1;
}

typedef struct _HandleRequestArgs
{
	char *buffer;
	int len;
	HTCPCPServer *server;
	int new_socket;
} HandleRequestArgs;

// This function is for threads created in runServer. 
// It takes in a pointer to a HandleRequestArgs struct.
// It then handles the callback to the server program.
void *handleRequest(void *args)
{
	//Request *req = g
}

void *runServer(void *server)
{
	HTCPCPServer *s = (HTCPCPServer *)server;
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	char *buffer = calloc(BUFFER_SIZE,sizeof(char));
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		printf("Socket failed\n");
		return NULL;
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	if(bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
	{
		printf("Binding socket failed\n");
		return NULL;
	}
	while(1)
	{
		if(listen(server_fd, MAX_THREADS_HTCPCP_SERVER) < 0)
		{
			printf("Error while listening\n");
			return NULL;
		}
		if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
		{
			printf("Error while accepting connection\n");
			return NULL;
		}
		valread = read(new_socket, buffer, BUFFER_SIZE);
		HandleRequestArgs *args = malloc(sizeof(HandleRequestArgs));
		args->buffer = calloc(valread+1, sizeof(char));
		memcpy(args->buffer, buffer, valread);
		args->len = valread;
		args->server = s;
		args->new_socket = new_socket;
		pthread_t *handler = malloc(sizeof(pthread_t));
		pthread_create(handler, NULL, handleRequest, args);
	}
}

void startServer(HTCPCPServer *server)
{
	pthread_mutex_lock(&(server->lock));
	pthread_create(&(server->mainThread), NULL, runServer, server);
	pthread_mutex_unlock(&(server->lock));
}

int main()
{
}
