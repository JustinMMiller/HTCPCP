#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared.h"


Headers* createHeaders(){
    Headers *h = malloc(sizeof(Headers));
    h->key = malloc(sizeof(char*)*10);
    h->value = malloc(sizeof(char*)*10);
    h->length = 0;
    return h;
}

int setHeader(Headers *headers, char *_key, char *_value){
    // Malloc a place for key and value since they'll be sticking around a while
    char *key = malloc(strlen(_key)+1); strcpy(key, _key);
    char *value = malloc(strlen(_value)+1); strcpy(value, _value);

    // Check if the key already exists, if it does then update it
    for(int i = 0; i < headers->length; i++){
        if(!strcmp(headers->key[i], key)){
            headers->value[i] = value;
            return headers->length;
        }
    }
    // If we have a multiple of 10 headers and are about to add another,
    // allocate space for the next 10
    if(headers->length % 10 == 0){
        headers->key = realloc(headers->key, sizeof(char*)*(headers->length+10));
        headers->value = realloc(headers->value, sizeof(char*)*(headers->length+10));
    }
    // If the header doesn't already exist, add it to the end
    headers->key[headers->length] = key;
    headers->value[headers->length] = value;
    headers->length = headers->length + 1;
    return headers->length;
}

char * getHeader(Headers *headers, char *key){
    for(int i = 0; i < headers->length; i++){
        if(!strcmp(headers->key[i], key)){
            return headers->value[i];
        }
    }
    return NULL;
}

// When passed in the string of headers, it will populate the Headers struct.
void populateHeadersFromString(Headers *headers, char *str)
{
	char *token = strtok(str, "\r\n");
	while(token != NULL)
	{
		char *key = strtok(NULL, ": ");
		char *value = strtok(NULL, "\r\n");
		setHeader(headers, key, value);
		token = strtok(NULL, "\r\n");
	}
}

char* requestToString(Request *req){
    char* string = malloc(sizeof(char)*(14+3+strlen(req->route)));
    sprintf(string, "%i %s HTCPCP/1.0\n", req->method, req->route);
    for(int i = 0; i < req->headers->length; i++){
        char *buf = malloc(sizeof(char)*(4+strlen(req->headers->key[i])+strlen(req->headers->value[i])));
        sprintf(buf, "%s: %s\n", req->headers->key[i], req->headers->value[i]);
        strcat(string, buf);
    }
    return string;
}

Request *requestFromString(char *str)
{
	Request *ret  = malloc(sizeof(Request));
	char *token = strtok(str, " ");
	ret->method  = atoi(token);
	char *troute = strtok(NULL, " ");
	ret->route = malloc(strlen(troute)+1);
	memcpy(ret->route, troute, strlen(troute));
	ret->headers = createHeaders();
	strtok(NULL, "\r\n");
	char *headerstart = strtok(NULL, "\r\n");
	populateHeadersFromString(ret->headers, headerstart);
	ret->bodyLength = atoi(getHeader(ret->headers, "Content-Length"));
	ret->body = malloc(ret->bodyLength+1);
	memcpy(ret->body, strtok(NULL, "\r\n\r\n"), ret->bodyLength);
	return ret;
}

char *responseToString(Response *res)
{
}
Response* responseFromString(char* string){
    char str[strlen(string)+1];
    strcpy(str, string);
    // Init response object
    Response *res = malloc(sizeof(Response));

    // Parse out status code
    strtok(str, " ");
    res->status = atoi(strtok(NULL, " "));
    strtok(NULL, "\n");
    printf("%i\n", res->status);
    // Rest of lines should contain one header each
    res->headers = createHeaders();
    char *key = strtok(NULL, ": ");
    char *value = strtok(NULL, "\n");
    while(key != NULL){
        // printf("%s: %s\n", key, value);
        setHeader(res->headers, key, value);
        printf("%s: %s\n", res->headers->key[res->headers->length-1], res->headers->value[res->headers->length-1]);
        key = strtok(NULL, ": ");
        value = strtok(NULL, "\n");
    }
    return res;
}
