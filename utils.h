#define STATUS_OK               200
#define STATUS_NOT_ACCEPTABLE   406
#define STATUS_TEAPOT           418

#define METHOD_GET              1
#define METHOD_POST             2
#define METHOD_BREW             4
#define METHOD_PROPFIND         8
#define METHOD_WHEN             16


typedef struct _Headers {
    char **key;
    char **value;
    int length;
} Headers;
Headers* createHeaders(){
    Headers *h = malloc(sizeof(Headers));
    h->key = malloc(sizeof(char*)*10);
    h->value = malloc(sizeof(char*)*10);
    h->length = 0;
    return h;
}
int setHeader(Headers *headers, char *key, char *value){
    // Check if the key already exists, if it does then update it
    for(int i = 0; i < headers->length; i++){
        if(!strcmp(headers->key[i], key)){
            printf("HESOIGBDNJK\n");
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
char* getHeader(Headers *headers, char *key){
    for(int i = 0; i < headers->length; i++){
        if(!strcmp(headers->key[i], key)){
            return headers->value[i];
        }
    }
    return NULL;
}

typedef struct _Request {
    int method;
    char* route;
    Headers headers;
    char *body;
    int bodyLength;
} Request;
char* requestToString(Request *req){
    char* string = malloc(sizeof(char)*(14+3+strlen(req->route)));
    sprintf(string, "%i %s HTCPCP/1.0\n", req->method, req->route);
    for(int i = 0; i < req->headers.length; i++){
        char *buf = malloc(sizeof(char)*(4+strlen(req->headers.key[i])+strlen(req->headers.value[i])));
        sprintf(buf, "%s: %s\n", req->headers.key[i], req->headers.value[i]);
        strcat(string, buf);
    }
    return string;
}

typedef struct _Response {
    int status;
    char *body;
    int bodyLength;
} Response;
