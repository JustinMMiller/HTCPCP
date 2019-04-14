#ifndef HTCPCP_CLIENT_H
#define HTCPCP_CLIENT_H

#include "../shared/shared.h"

/* Takes in a Request and opens a socket to ship it off to its destination */
Response* sendRequest(Request *req);

/* Splits a URL into its host and route parts, i.e. 'coffee://{HOST}/{ROUTE}' */
void getURLParts(char *_url, char **dest_host, char **dest_route);

/* Performs a GET request to the given url */
Response* get(char *url, Headers *headers);

/* Performs a BREW request to the given url */
Response* brew(char *url, Headers *headers, char* body, int bodyLength);

/* DEPRECATED. Should be replaced with BREW, which functions identically */
Response* post(char *url, Headers *headers, char* body, int bodyLength);

/* Performs a PROPFIND request to the given url */
Response* propfind(char *url, Headers *headers);

/* Performs a WHEN request to the given url */
Response* when(char *url, Headers *headers);

/* Performs a DELETE request to the given url */
Response* delete(char *url, Headers *headers, char* body, int bodyLength);

#endif
