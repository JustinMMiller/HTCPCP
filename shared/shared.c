#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared.h"


Headers *createHeaders()
{
    Headers *h = calloc(1, sizeof(Headers));
    h->key = calloc(10, sizeof(char*));
    h->value = calloc(10, sizeof(char*));
    h->length = 0;
    return h;
}

int setHeader(Headers *headers, char *_key, char *_value)
{
    // Malloc a place for key and value since they'll be sticking around a while
    // Should there be a check to ensure that what's given is a valid header?
    char *key = calloc(strlen(_key)+1, sizeof(char));
    strcpy(key, _key);
    char *value = calloc(strlen(_value)+1, sizeof(char));
    strcpy(value, _value);

    // Check if the key already exists, if it does then update it
    for(int i = 0; i < headers->length; i++)
    {
        if(!strcmp(headers->key[i], key))
        {
            headers->value[i] = value;
            return headers->length;
        }
    }
    // If we have a multiple of 10 headers and are about to add another,
    // allocate space for the next 10
    if(headers->length > 0 && headers->length % 10 == 0)
    {
        headers->key = realloc(headers->key, sizeof(char*)*(headers->length)+10);
        headers->value = realloc(headers->value, sizeof(char*)*(headers->length)+10);
    }
    // If the header doesn't already exist, add it to the end
    headers->key[headers->length] = key;
    headers->value[headers->length] = value;
    headers->length = headers->length + 1;
    return headers->length;
}

char *getHeader(Headers *headers, char *key)
{
    for(int i = 0; i < headers->length; i++)
    {
        if(!strcmp(headers->key[i], key))
        {
            return headers->value[i];
        }
    }
    return NULL;
}

// When passed in the string of headers, it will populate the Headers struct.
void populateHeadersFromString(Headers *headers, char *str)
{
    char *str_tok;
    strtok_r(str, "\r\n", &str_tok);
    char *key = strtok_r(NULL, ": ", &str_tok);
    char *value = strtok_r(NULL, "\r\n", &str_tok);
    while(key != NULL)
    {
        setHeader(headers, key, value);
        key = strtok_r(NULL, ": ", &str_tok);
        value = strtok_r(NULL, "\r\n", &str_tok);
    }
}

char *requestToString(Request *req)
{
    char* string = malloc(sizeof(char)*(14+3+strlen(req->route)));
    sprintf(string, "%i %s HTCPCP/1.0\r\n", req->method, req->route);
    for(int i = 0; i < req->headers->length; i++)
    {
        char *buf = malloc(sizeof(char)*(4+strlen(req->headers->key[i])+strlen(req->headers->value[i])));
        sprintf(buf, "%s: %s\r\n", req->headers->key[i], req->headers->value[i]);
        string = strcat(string, buf);
    }
    return string;
}

Request *requestFromString(char *_str)
{
    char *str = malloc(strlen(_str)+1);
    char *str_tok;
    strcpy(str, _str);

    Request *ret  = malloc(sizeof(Request));
    char *token = strtok_r(str, " ", &str_tok);
    ret->method  = atoi(token);
    char *troute = strtok_r(NULL, " ", &str_tok);
    ret->route = malloc(strlen(troute)+1);
    strcpy(ret->route, troute);
    ret->headers = createHeaders();
    char *headerstart = strtok_r(NULL, "\n", &str_tok);
    populateHeadersFromString(ret->headers, headerstart);
    ret->bodyLength = atoi(getHeader(ret->headers, "Content-Length"));
    ret->body = malloc(ret->bodyLength+1);
    memcpy(ret->body, strtok_r(NULL, "\n", &str_tok), ret->bodyLength);
    return ret;
}

char *headersToString(Headers *headers)
{
    char *ret = calloc(4096, sizeof(char));
    int pos = 0;
    for(int i = 0; i < headers->length; i++)
    {
        strcpy(ret+pos, headers->key[i]);
        pos += strlen(headers->key[i]);
        ret[pos++] = ':';
        ret[pos++] = ' ';
        strcpy(ret+pos, headers->value[i]);
        pos += strlen(headers->value[i]);
        ret[pos++] = '\r';
        ret[pos++] = '\n';
    }
    ret[pos++] = '\r';
    ret[pos++] = '\n';
    return ret;
}

char *responseToString(Response *res)
{
    char *head = headersToString(res->headers);
    char *ret = malloc(res->bodyLength + strlen(head) + 64);
    int len = 0;
    sprintf(ret+len, "%s", "HTCPCP/1.0 ");
    len += strlen(ret);
    sprintf(ret+len, "%i", res->status);
    len = strlen(ret);
    sprintf(ret+len, "%s", "\r\n");
    len = strlen(ret);
    sprintf(ret+len, "%s", head);
    len += strlen(head);
    free(head);
    printf("START %s\n", res->body);
    sprintf(ret+len, "%s", res->body);
    return ret;
}

Response *responseFromString(char* string)
{
    char *str = malloc(strlen(string)+1);
    char *str_tok;
    strcpy(str, string);
    // Init response object
    Response *res = malloc(sizeof(Response));

    // Parse out status code
    strtok_r(str, " ", &str_tok);
    res->status = atoi(strtok_r(NULL, " ", &str_tok));
    strtok_r(NULL, "\r\n", &str_tok);

    // Parse header (up to "\r\n\r\n")
    char *head_end = strstr(string, "\r\n\r\n");
    int head_size = head_end - strstr(string, "\r\n");
    char *head = malloc(head_size+1);
    strncpy(head, strstr(string, "\r\n"), head_size);
    strcat(head, "\0");

    res->headers = createHeaders();
    populateHeadersFromString(res->headers, head);

    // Rest of lines should be the body
    res->body = head_end + 4;
    res->bodyLength = strlen(res->body);

    return res;
}

void destroyResponse(Response *res)
{
    if(res->body)
    {
        free(res->body);
    }
    destroyHeaders(res->headers);
    free(res);
}

void destroyHeaders(Headers *head)
{
    for(int i = 0; i < head->length; i++)
    {
        free(head->key[i]);
        free(head->value[i]);
    }
    free(head->key);
    free(head->value);
    free(head);
}