#include "htcpcp-server.h"

#include <string.h>
#include <stdlib.h>

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

int main()
{
}
