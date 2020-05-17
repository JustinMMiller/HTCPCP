#ifndef HTCPCP_SERVER_H
#define HTCPCP_SERVER_H

#include <pthread.h>
#include <semaphore.h>

#include "../shared/shared.h"

#define MAX_THREADS_HTCPCP_SERVER 32
#define BUFFER_SIZE 4096
#define CALLBACK_ARRAY_INITIAL_LENGTH 10


typedef Response *callbackFunc(Request*);

/**
 * @struct Callback
 * Internal structure use to represent HTCPCP
 * server function callbacks.
 */
typedef struct _Callback
{
    char *url; //!< The endpoint a Callback is registered to. 
    int method; //!< The method (GET, BREW, etc.) this callback is for.
    callbackFunc *callback; //!< The function to call for this endpoint method combo.
} Callback;

/**
 * @struct HTCPCPServer
 * This struct represents a HTCPCPServer.
 * It should be controlled with the appropriate functions,
 * and not manipulated directly.
 */
typedef struct _HTCPCPServer
{
    Callback *callbacks; //!< An array of Callback structs.
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
// $$TODO Add a deleteRoute and an updateRoute method.

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
void *runServer(void *server);	// Main thread function.

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
