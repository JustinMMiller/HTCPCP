#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "htcpcp-client.h"

/**
 * Takes in a Request and opens a socket to ship it off to its destination 
 * @param req The request to be sent.
 */
Response *sendRequest(Request *req)
{
    // $$GLOBAL Check return values
    char *uri = getHeader(req->headers, "HOST");

    char *buf = calloc(sizeof(char), 1024);
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *payload = NULL;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return NULL;
    }

    memset(&serv_addr, '\0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, uri, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return NULL;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return NULL;
    }
    payload = requestToString(req);
    send(sock, payload, strlen(payload), 0);
    read(sock, buf, 1024);
    Response *res = responseFromString(buf);
    free(buf);
    return res;
}

/**
 * Splits a URL into its host and route parts, i.e. 'coffee://{HOST}/{ROUTE}' 
 * @param _url the url to be split.
 * @param dest_host a pointer to the char * to hold the host string
 * @param dest_route a pointer to the char * to hold the route string.
 */
void getURLParts(char *_url, char **dest_host, char **dest_route)
{
    // Copy url to make it usable in strtok
    // $$GLOBAL Safer string checking
    char *url = malloc(strlen(_url) + 1);
    char *url_tok;
    strcpy(url, _url);

    // $$TODO ensure url is of the form coffee://{HOST}/{ROUTE}
    // rather than ignoring the first part.
    char delim[] = "/";
    strtok_r(url, delim, &url_tok);

    // Extract the host and route from the remaining parts of the url
    // coffee://{HOST}/{ROUTE}
    strcpy(*dest_host, strtok_r(NULL, delim, &url_tok));

    char *temp = strtok_r(NULL, "", &url_tok);
    if (temp != NULL)
    {
        char *route = malloc(sizeof(char) * strlen(temp) + 2);
        route[0] = '/';
        strcat(route, temp);
        *dest_route = route;
    }
    else
    {
        *dest_route = "/";
    }
}

/**
 * 
 */
void initRequest(Request *req, char *route,
                 Headers *headers, char *body, int bodyLength)
{
    req->body = body;
    req->bodyLength = bodyLength;
    req->headers = headers;
    req->route = route;
}

// BLOCKING CALL
// Can this be made non-blocking?
Response *get(char *url, Headers *headers)
{
    char *host = malloc(strlen(url));
    char *route = malloc(strlen(url));
    getURLParts(url, &host, &route);
    // $$GLOBAL Check return values

    if (headers == NULL)
        headers = createHeaders();
    setHeader(headers, "HOST", host);
    setHeader(headers, "Content-Length", "0");

    Request *req = malloc(sizeof(Request));
    initRequest(req, route, headers, NULL, 0);
    req->method = METHOD_GET;
    return sendRequest(req);
}

// BLOCKING CALL
// Can this be made non-blocking?
Response *brew(char *url, Headers *headers, char *body, int bodyLength)
{
    char *host = malloc(strlen(url));
    char *route = malloc(strlen(url));
    getURLParts(url, &host, &route);

    if (headers == NULL)
        headers = createHeaders();
    setHeader(headers, "HOST", host);
    setHeader(headers, "Content-Length", "0");

    Request *req = malloc(sizeof(Request));
    initRequest(req, route, headers, body, bodyLength);
    req->method = METHOD_BREW;
    return sendRequest(req);
}

// BLOCKING CALL
// Can this be made non-blocking?
Response *post(char *url, Headers *headers, char *body, int bodyLength)
{
    return brew(url, headers, body, bodyLength);
}

// BLOCKING CALL
// Can this be made non-blocking?
Response *delete(char *url, Headers *headers, char *body, int bodyLength)
{
    char *host = malloc(strlen(url));
    char *route = malloc(strlen(url));
    getURLParts(url, &host, &route);

    if (headers == NULL)
        headers = createHeaders();
    setHeader(headers, "HOST", host);
    setHeader(headers, "Content-Length", "0");

    Request *req = malloc(sizeof(Request));
    initRequest(req, route, headers, body, bodyLength);
    req->method = METHOD_DELETE;
    return sendRequest(req);
}
