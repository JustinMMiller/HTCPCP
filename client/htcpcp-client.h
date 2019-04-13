#ifndef HTCPCP_CLIENT_H
#define HTCPCP_CLIENT_H

#include "../shared/shared.h"

Response* sendRequest(Request *req);

Response* get(char *url, Headers *headers);

Response* post(char *url, Headers *headers, char* body, int bodyLength);

#endif
