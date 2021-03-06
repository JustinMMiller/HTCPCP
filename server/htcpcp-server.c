#include <arpa/inet.h>
#include <netinet/in.h>
#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "htcpcp-server.h"

HTCPCPServer *getServer()
{
    HTCPCPServer *server = malloc(sizeof(HTCPCPServer));
    server->callbacks = calloc(CALLBACK_ARRAY_INITIAL_LENGTH, sizeof(Callback));
    pthread_mutex_init(&(server->lock), NULL);
    sem_init(&(server->children), 0, MAX_THREADS_HTCPCP_SERVER);
    server->numCallbacks = 0;
    server->maxCallbacks = CALLBACK_ARRAY_INITIAL_LENGTH;
    server->impl_methods = 0;
    server->address = NULL;
    return server;
}

void setServerAddress(HTCPCPServer *server, char *address)
{
    // $$GLOBAL strncpy
    server->address = malloc(strlen(address)+1);
    strcpy(server->address, address);
}

void addRoute(HTCPCPServer *server, int method, char *url, callbackFunc *callback)
{
    // $$TODO What should be done in the event of a duplicate method/url combo?
    pthread_mutex_lock(&(server->lock));
    int n = server->numCallbacks;
    if(server->numCallbacks < server->maxCallbacks)
    {
        server->callbacks[n].callback = callback;
        server->callbacks[n].method = method;
        char *str = malloc(strlen(url) + 1);
        strcpy(str, url);
        server->callbacks[n].url = str;
        server->numCallbacks++;
        server->impl_methods |= method;
        pthread_mutex_unlock(&(server->lock));
        return;
    }
    else
    {
        Callback *temp = realloc(server->callbacks, 
                             sizeof(Callback) * (n + (n/2)));
        if(temp)
        {
            memset(temp+n, 0, sizeof(Callback) * (n/2));
            free(server->callbacks);
            server->callbacks = temp;
            server->maxCallbacks = (n + (n/2));
            pthread_mutex_unlock(&(server->lock));
            addRoute(server, method, url, callback);
        }
        else
        {
            printf("Unable to allocate more space for ");
            printf("more callbacks, leaving existing callbacks ");
            printf("in place\n");
        }
        
    }
}

int getCallbackIndex(HTCPCPServer *server, int method, char *callbackURL)
{
    pthread_mutex_lock(&(server->lock));
    for(int i = 0; i < server->numCallbacks; i++)
    {
        if(server->callbacks[i].method == method &&
                !strcmp(callbackURL, server->callbacks[i].url))
        {
            pthread_mutex_unlock(&(server->lock));
            return i;
        }
    }
    pthread_mutex_unlock(&(server->lock));
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
    HandleRequestArgs *a = (HandleRequestArgs *)args;
    sem_wait(&(a->server->children));
    Request *req = requestFromString(a->buffer);
    if(req == NULL)
    {
        Response *res = malloc(sizeof(Response));
        res->status = STATUS_NOT_FOUND;
        res->headers = createHeaders();
        setHeader(res->headers, "Safe", "no");
        res->body = malloc(20);
        strcpy(res->body, "You Done goofed\0");
        res->bodyLength = strlen(res->body+1);
        char *str = responseToString(res);
        send(a->new_socket, str, strlen(str), 0);
        destroyResponse(res);
        sem_post(&(a->server->children));
        return NULL;
    }
    printf("%s\n", requestToString(req));
    // $$TODO This should be refactored to be processCallback(server, method, route.)
    int callbackIndex = getCallbackIndex(a->server, req->method, req->route);
    if(callbackIndex < 0)
    {
        printf("Unable to find a callback for route %s\n", req->route);
        Response *res = malloc(sizeof(Response));
        if(!(a->server->impl_methods & req->method))
        {
            res->status = STATUS_NOT_IMPL;
        }
        else
        {
            res->status = STATUS_NOT_FOUND;
        }
        res->headers = createHeaders();
        setHeader(res->headers, "Safe", "no");
        res->body = malloc(20);
        strcpy(res->body, "You Done goofed\0");
        res->bodyLength = strlen(res->body)+1;
        char *str = responseToString(res);
        send(a->new_socket, str, strlen(str), 0);
        destroyResponse(res);
        sem_post(&(a->server->children));
        return NULL;
    }
    pthread_mutex_lock(&(a->server->lock));
    // $$TODO Finding the callback and calling it should be
    //  in the same critical section
    Response *res = (*(a->server->callbacks[callbackIndex].callback))(req);
    pthread_mutex_unlock(&(a->server->lock));
    char *str = responseToString(res);
    send(a->new_socket, str, strlen(str), 0);
    sem_post(&(a->server->children));
    return NULL;
}

void *runServer(void *server)
{
    HTCPCPServer *s = (HTCPCPServer *)server;
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1;
    char *buffer = calloc(BUFFER_SIZE,sizeof(char));
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        printf("Socket failed\n");
        return NULL;
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(s->address);
    address.sin_port = htons(PORT);
    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        printf("Binding socket failed\n");
        return NULL;
    }
    printf("Socket bound on port %i\n", PORT);
    while(1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        int numKids = 0;
        sem_getvalue(&(s->children), &numKids);
        if(!numKids)
        {
            continue;
        }
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
    // $$TODO Ensure all fields of server are in a valid state.
    if(server->address == NULL)
    {
        setServerAddress(server, "127.0.0.1");
    }
    pthread_mutex_lock(&(server->lock));
    pthread_create(&(server->mainThread), NULL, runServer, server);
    pthread_mutex_unlock(&(server->lock));
}

