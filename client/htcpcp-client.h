#ifndef HTCPCP_CLIENT_H
#define HTCPCP_CLIENT_H

#include "../shared/shared.h"

int sendRequest(Request *req);

Response* get(char *url, Headers *headers);

#endif
