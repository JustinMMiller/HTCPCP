#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "htcpcp-client.h"
#include "../utils.h"


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

    get("coffee://test.com/fuck/hello", NULL);

    return 0;
}
