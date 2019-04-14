#ifndef HTCPCP_SERVER_H
#define HTCPCP_SERVER_H

#include "../shared/shared.h"
#include <pthread.h>
#include <semaphore.h>

typedef Response * callbackFunc(Request*);

typedef struct _HTCPCPServer
{
	// The following 3 arrays are kept in lockstep such that index
	// i from each array forms the total information of the ith callback.
	// Yes, callbacks is an array of function pointers. I'm looking forward to it.
	char **callbackURLs;
	int *callbackMethods;
	callbackFunc **callbacks;
	int numCallbacks;
	int maxCallbacks;
	pthread_t mainThread;
	sem_t children;
	pthread_mutex_t lock;
	int impl_methods;
} HTCPCPServer;

// Public functions
void addRoute(HTCPCPServer *server, int method, char *url, callbackFunc *callback);
HTCPCPServer *getServer();
void startServer(HTCPCPServer *server);
void stopServer(HTCPCPServer *server);

// Internal functions
void *runServer(void *server); 	// Main thread function.
void *handleRequest(void *args);
int getCallbackIndex(HTCPCPServer *server, int method, char *callbackURL);

#endif
