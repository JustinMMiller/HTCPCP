#define STATUS_OK               200
#define STATUS_NOT_ACCEPTABLE   406
#define STATUS_TEAPOT           418


typedef struct _Header {
    char *key;
    char *value;
} Header;

typedef struct _Request {
    Header *header;
    int numHeaders;
    char *body;
    int bodyLength;
} Request;

typedef struct _Response {
    int status;
    char *body;
    int bodyLength;
} Response;
