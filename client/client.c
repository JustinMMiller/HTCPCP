#include "htcpcp-client.h"
#include <stdlib.h>
#include <stdio.h>

int main()
{
    Response *res = post("coffee://127.0.0.1/brew", NULL, "a", 1);
    printf("Received response : %s\n", res->body);

    return 0;
}
