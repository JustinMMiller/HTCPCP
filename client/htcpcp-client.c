#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "htcpcp-client.h"


Response* sendRequest(Request *req){
    char *uri = getHeader(req->headers, "HOST");

    int sock = 0;
    struct sockaddr_in serv_addr;

    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\n Socket creation error \n");
        return NULL;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, uri, &serv_addr.sin_addr) <= 0){
        printf("\nInvalid address/ Address not supported \n");
        return NULL;
    }

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        printf("\nConnection Failed \n");
        return NULL;
    }
    
    char *payload = requestToString(req);
    printf("---------------------------------------\n");
    printf("SENDING PAYLOAD:\n%s\n", payload);
    printf("---------------------------------------\n");
    char buf[1024] = {0};
    send(sock, payload, strlen(payload), 0);
    printf("Request sent!\n");
    read(sock, buf, 1024);
    printf("Response received:\n%s\n", buf);
    printf("---------------------------------------\n");
    Response *res = responseFromString(buf);
    return res;
}

void getURLParts(char *_url, char **dest_host, char **dest_route){
    // Copy url to make it usable in strtok
    char url[strlen(_url)+1]; strcpy(url, _url);

    // Strip away the unnecessary beginning of the url
    char delim[] = "/";
    strtok(url, delim);

    // Extract the host and route from the remaining parts of the url
    // coffee://{HOST}/{ROUTE}
    strcpy(*dest_host, strtok(NULL, delim));

    char *temp = strtok(NULL, "");
    if(temp != NULL){
        char *route = malloc(sizeof(char)*strlen(temp)+2);
        route[0] = '/';
        strcat(route, temp);
        *dest_route = route;
    }
    else{
        *dest_route = "/";
    }
}

/**
 * 
 */
Response* get(char *url, Headers *headers){
    char *host = malloc(strlen(url));
    char *route = malloc(strlen(url));
    getURLParts(url, &host, &route);

    if(headers == NULL)
        headers = createHeaders();
    setHeader(headers, "HOST", host);
    setHeader(headers, "Content-Length", "0");

    Request req = {
        .method = METHOD_GET,
        .route = route,
        .headers = headers,
        .body = NULL,
        .bodyLength = 0
    };
    return sendRequest(&req);
}

Response* brew(char *url, Headers *headers, char* body, int bodyLength){
    char *host = malloc(strlen(url));
    char *route = malloc(strlen(url));
    getURLParts(url, &host, &route);

    if(headers == NULL)
        headers = createHeaders();
    setHeader(headers, "HOST", host);

    Request req = {
        .method = METHOD_POST,
        .route = route,
        .headers = headers,
        .body = body,
        .bodyLength = bodyLength
    };
    return sendRequest(&req);
}

Response* post(char *url, Headers *headers, char* body, int bodyLength){
    return brew(url, headers, body, bodyLength);
}

