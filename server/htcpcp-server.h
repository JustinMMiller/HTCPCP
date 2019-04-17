#ifndef HTCPCP_SERVER_H
#define HTCPCP_SERVER_H

#include "../shared/shared.h"
#include <pthread.h>
#include <semaphore.h>

typedef Response * callbackFunc(Request*);

/**
 * @struct _HTCPCPServer
 * This struct represents a HTCPCPServer.
 */
typedef struct _HTCPCPServer
{
	// The following 3 arrays are kept in lockstep such that index
	// i from each array forms the total information of the ith callback.
	// Yes, callbacks is an array of function pointers. I'm looking forward to it.
	char **callbackURLs; //!< The URLs of registered callbacks.
	int *callbackMethods; //!< The HTCPCP function of the registered callbacks
	callbackFunc **callbacks; //!< The function pointers to the registered callbacks
	int numCallbacks; //!< The number of callbacks currently registered
	int maxCallbacks; //!< The max number of callbacks the server can currently hold
	pthread_t mainThread; //!< The pthread representing the main server thread
	sem_t children; //!< A semaphore to limit the number of concurrent connections
	pthread_mutex_t lock; //!< A lock for use with the server object internally.
	int impl_methods; //!< A bitmask of methods the server callbacks implement
        char *address; //!< The address the server is bound to.
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
 * @param server The HTCPCPServer instance
 * @param address The address to bind to
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
