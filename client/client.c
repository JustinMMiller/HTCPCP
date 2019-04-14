#include "htcpcp-client.h"
#include <stdlib.h>

int main(){
    Response *res = get("coffee://127.0.0.1/brew", NULL);

    return 0;
}
