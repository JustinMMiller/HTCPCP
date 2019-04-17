#ifndef HTCPCP_SHARED_H
#define HTCPCP_SHARED_H

#define STATUS_OK               200
#define STATUS_NOT_FOUND        404
#define STATUS_NOT_ACCEPTABLE   406
#define STATUS_TEAPOT           418
#define STATUS_NOT_IMPL         501

#define METHOD_GET              1
#define METHOD_POST             2
#define METHOD_BREW             4
#define METHOD_PROPFIND         8
#define METHOD_WHEN             16
#define METHOD_HEAD             32
#define METHOD_PUT              64
#define METHOD_DELETE           128
#define METHOD_CONNECT          256
#define METHOD_OPTIONS          512
#define METHOD_TRACE            1024

#define PORT 489

/**
 * @struct Headers
 * This struct represents the headers of a HTCPCP Request or Response
 * @member key The list of keys for the headers
 * @member value The list of values for the headers
 * @member length The number of entries in the Headers
 */
typedef struct _Headers {
    char **key;
    char **value;
    int length;
} Headers;
/**
 * This function instantiates a Headers struct and returns it.
 * @return An instance of a Headers struct.
 */
Headers* createHeaders();
/**
 * Returns the value of a given header
 * @param headers The Headers struct to look in.
 * @param key The key of the header to return
 * @return The value of the header with the given key.
 */
char * getHeader(Headers *headers, char *key);
/**
 * Sets the header with the given key to the given value.
 * @param headers The Headers struct to insert into/update.
 * @param key The key to insert/update.
 * @param value The value to set the header to.
 * @return The new length of the Headers struct.
 */
int setHeader(Headers *headers, char *key, char *value);

/**
 * @struct Request
 * This struct represents a HTCPCP request.
 * @member method The HTCPCP method of the request
 * @member route The route the request was sent to.
 * @member headers The Headers of the request
 * @member body The content of the request
 * @method bodyLength The length of the content of the request
 */
typedef struct _Request {
    int method;
    char* route;
    Headers *headers;
    char *body;
    int bodyLength;
} Request;
/**
 * This function takes in a Request and creates a string representation of it.
 * @param req The Request to format to a string
 * @return A String representation of the given Request
 */
char* requestToString(Request *req);
/**
 * This function takes in a string and returns it as a Request.
 * @param str A string representation of a Request
 * @return A Request struct representation of the given string
 */
Request *requestFromString(char *str);

/**
 * @struct Response
 * This struct represents a HTCPCP response
 * @member status The status code of the response
 * @member headers The headers of the response
 * @member body The content of the response
 * @member bodyLength The length of the content of the response
 */
typedef struct _Response {
    int status;
    Headers *headers;
    char *body;
    int bodyLength;
} Response;
/**
 * This function takes in a Response and creates a string representation of it.
 * @param req The Response to format to a string
 * @return A String representation of the given Response
 */
char *responseToString(Response *res);
/**
 * This function takes in a string and returns it as a Response.
 * @param str A string representation of a Response
 * @return A Response struct representation of the given string
 */
Response* responseFromString(char* string);

#endif
