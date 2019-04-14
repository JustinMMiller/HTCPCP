#include "htcpcp-server.h"

Response * myCallback(Request *req)
{
	Response *res = malloc(sizeof(Response));
	res->status = STATUS_OK;
	res->headers = createHeaders();
	setHeader(res->headers, "Safe", "yes");
	res->body = malloc(20);
	strcpy(res->body, "Hello World\0");
	res->bodyLength = strlen(res->body+1);
	return res;
}

int main()
{
	HTCPCPServer *server = getServer();
	addRoute(server, METHOD_GET, "/brew", myCallback);
	startServer(server);
	while(1){sleep(1);printf("Yawn\n");}
}
