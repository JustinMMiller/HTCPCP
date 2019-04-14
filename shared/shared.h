#ifndef HTCPCP_SHARED_H
#define HTCPCP_SHARED_H

#define STATUS_OK               200
#define STATUS_NOT_FOUND        404
#define STATUS_NOT_ACCEPTABLE   406
#define STATUS_TEAPOT           418
#define STATUS_NOT_IMPL         501

#define METHOD_GET              1
#define METHOD_POST             2
#define METHOD_BREW             4
#define METHOD_PROPFIND         8
#define METHOD_WHEN             16
#define METHOD_HEAD             32
#define METHOD_PUT              64
#define METHOD_DELETE           128
#define METHOD_CONNECT          256
#define METHOD_OPTIONS          512
#define METHOD_TRACE            1024

#define PORT 489

typedef struct _Headers {
    char **key;
    char **value;
    int length;
} Headers;
Headers* createHeaders();
char * getHeader(Headers *headers, char *key);
int setHeader(Headers *headers, char *key, char *value);

typedef struct _Request {
    int method;
    char* route;
    Headers *headers;
    char *body;
    int bodyLength;
} Request;
char* requestToString(Request *req);
Request *requestFromString(char *str);

typedef struct _Response {
    int status;
    Headers *headers;
    char *body;
    int bodyLength;
} Response;
char *responseToString(Response *res);
Response* responseFromString(char* string);

#endif
