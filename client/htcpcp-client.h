#ifndef HTCPCP_CLIENT_H
#define HTCPCP_CLIENT_H

#include "../shared/shared.h"

/**
 * Performs a GET request to the given url 
 * @param url the url to send the request to
 * @param headers The Headers to send with the request.
 */
Response* get(char *url, Headers *headers);

/**
 * Performs a BREW request to the given url 
 * @param url The url to send the BREW request to.
 * @param headers The Headers to send with the request.
 * @param body The content of the request
 * @param bodyLength The length of the content body
 */
Response* brew(char *url, Headers *headers, char* body, int bodyLength);

/*
 * @deprecated. Should be replaced with BREW, which functions identically 
 * @param url The url to send the POST request to.
 * @param headers The Headers to send with the request.
 * @param body The content of the request
 * @param bodyLength The length of the content body
 */
Response* post(char *url, Headers *headers, char* body, int bodyLength);

/**
 * Performs a PROPFIND request to the given url 
 * @param url The url to send the PROPFIND request to.
 * @param headers The Headers to send with the request.
 */
Response* propfind(char *url, Headers *headers);

/**
 * Performs a WHEN request to the given url 
 * @param url The url to send the WHEN request to.
 * @param headers The Headers to send with the request.
 */
Response* when(char *url, Headers *headers);

/**
 * Performs a DELETE request to the given url 
 * @param url The url to send the DELETE request to.
 * @param headers The Headers to send with the request.
 * @param body The content of the request
 * @param bodyLength The length of the content body
 */
Response* delete(char *url, Headers *headers, char* body, int bodyLength);

#endif
