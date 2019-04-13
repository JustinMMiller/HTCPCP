#ifndef HTCPCP_SHARED_H
#define HTCPCP_SHARED_H

#define STATUS_OK               200
#define STATUS_NOT_ACCEPTABLE   406
#define STATUS_TEAPOT           418

#define METHOD_GET              1
#define METHOD_POST             2
#define METHOD_BREW             4
#define METHOD_PROPFIND         8
#define METHOD_WHEN             16

#define PORT 489

typedef struct _Headers {
    char **key;
    char **value;
    int length;
} Headers;
Headers* createHeaders();
char* getHeader(Headers *headers, char *key);
int setHeader(Headers *headers, char *key, char *value);

typedef struct _Request {
    int method;
    char* route;
    Headers headers;
    char *body;
    int bodyLength;
} Request;
char* requestToString(Request *req);

typedef struct _Response {
    int status;
    char *body;
    int bodyLength;
} Response;
char *responseToString(Response *res);

#endif
