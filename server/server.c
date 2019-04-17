//#include <wiringPi.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "htcpcp-server.h"

int position = 0;

int state = 0;

Response * deleteCallback(Request *req)
{
	Response *res = malloc(sizeof(Response));
	res->status = STATUS_NOT_IMPL;
	res->headers = createHeaders();
	setHeader(res->headers, "Safe", "no");
	res->body = malloc(64);
	strcpy(res->body, "That operation is not supported here, chump.\0");
	res->bodyLength = strlen(res->body+1);
	return res;
}

Response * brewCallback(Request *req)
{
	Response *res = malloc(sizeof(Response));
	res->status = STATUS_OK;
	res->headers = createHeaders();
	setHeader(res->headers, "Safe", "yes");
	res->body = malloc(64);
	strcpy(res->body, "Starting some coffee for you.\0");
	res->bodyLength = strlen(res->body+1);
	return res;
}

Response * postCallback(Request *req)
{
	return brewCallback(req);
}

int main(int argc, char **argv)
{
	HTCPCPServer *server = getServer();
	addRoute(server, METHOD_BREW, "/brew", brewCallback);
	addRoute(server, METHOD_POST, "/brew", postCallback);
	addRoute(server, METHOD_DELETE, "/brew", deleteCallback);
	if(argc > 1)
	{
		setServerAddress(server, argv[1]);
	}
	startServer(server);
	while(1)
	{
		sleep(1);
		printf("Yawn\n");
	}
}
