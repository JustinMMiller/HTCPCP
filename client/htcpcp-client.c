#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "htcpcp-client.h"


int sendRequest(Request *req){
    char *host = getHeader(&req->headers, "HOST");
    int sock = 0;
    struct sockaddr_in serv_addr;

    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, host, &serv_addr.sin_addr) <= 0){
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        printf("\nConnection Failed \n");
        return -1;
    }
    
    char *payload = requestToString(req);
    char *buf[1024] = {0};
    send(sock, payload, strlen(payload), 0);
    printf("Request sent\n");
    read(sock, buf, 1024);
    printf("Response received: %s\n", buf);

    return 0;
}

/**
 * 
 */
Response* get(char *url, Headers *headers){
    // Copy url to make it usable in strtok
    char url2[strlen(url)+1];
    strcpy(url2, url);

    // Strip away the unnecessary beginning of the url
    char delim[] = "/";
    strtok(url2, delim);

    // Extract the host and route from the remaining parts of the url
    // coffee://{HOST}/{ROUTE}
    char *host = strtok(NULL, delim);
    printf("%s\n", host);
    char *temp = strtok(NULL, "");
    char *route = malloc(sizeof(char)*strlen(temp)+2);
    route[0] = '/';
    strcat(route, temp);
    printf("%s\n", route);

    if(headers == NULL){
        headers = createHeaders();
    }
    setHeader(headers, "HOST", host);

    Request req = {
        .method = METHOD_GET,
        .route = route,
        .headers = *headers,
        .body = NULL,
        .bodyLength = 0
    };
    sendRequest(&req);
    return NULL;
}

int main(){
    // Headers *h = createHeaders();
    // setHeader(h, "hello", "world");
    // setHeader(h, "iam", "sotired");

    // Request req = {
    //     .method = METHOD_GET,
    //     .route = "/fuck/town/its5am",
    //     .headers = *h,
    //     .body = NULL,
    //     .bodyLength = 0
    // };
    // printf("%s\n", requestToString(&req));

    get("coffee://127.0.0.1/fuck/hello", NULL);

    return 0;
}
