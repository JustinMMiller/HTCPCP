#ifndef HTCPCP_SERVER_H
#define HTCPCP_SERVER_H

#include "../shared/shared.h"
#include <pthread.h>
#include <semaphore.h>

typedef Response * callbackFunc(Request*);

/**
 * @struct HTCPCPServer
 * This struct represents a HTCPCPServer.
 * @member callbackURLs The URLs of registered callbacks.
 * @member callbackMethods The HTCPCP function of the registered callbacks
 * @member callbacks The function pointers to the registered callbacks
 * @member numCallbacks The number of callbacks currently registered
 * @member maxCallbacks The max number of callbacks the server can currently hold
 * @member mainThread The pthread representing the main server thread
 * @member children A semaphore to limit the number of concurrent connections
 * @member lock A lock for use with the server object internally.
 * @member impl_methods A bitmask of methods the server callbacks implement
 * @member address The address the server is bound to.
 */
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
        char *address;
} HTCPCPServer;

// Public functions
/**
 * Registers the given callback function for the specified method at the given url
 * @param server The HTCPCPServer instance
 * @param method The HTCPCP method that this callback handles
 * @param url The url relative to the website this callback is attached to. IE /brew
 * @param callback The function to call when a request of the given method is received at the given url
 */
void addRoute(HTCPCPServer *server, int method, char *url, callbackFunc *callback);
/**
 * This function instantiates an HTCPCPServer instance and returns it.
 * @return A prepared HTCPCPServer reference.
 */
HTCPCPServer *getServer();
/**
 * This function starts the server running.
 * @param server The HTCPCPServer instance
 */
void startServer(HTCPCPServer *server);
/**
 * This function sets the address the server is attached to.
 */
void setServerAddress(HTCPCPServer *server, char *address);

// Internal functions
/**
 * Internal helper function. This is the function the mainThread runs.
 */
void *runServer(void *server); 	// Main thread function.
/**
 * Internal helper function. This is the function called by helper threads when a request comes in.
 * It parses out the Request and calls the appropriate callback function.
 */
void *handleRequest(void *args);
/**
 * Internal helper function. This finds the index of a callbacks.
 * @returns The index of the callback, -1 if it doesn't exist.
 */
int getCallbackIndex(HTCPCPServer *server, int method, char *callbackURL);

#endif
