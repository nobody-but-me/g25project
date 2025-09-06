

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "./include/socket.h"

int init_server(struct sockaddr_in* address)
{
    int opt = 1;
    int server;
    
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) {
	perror("[FAILED] : Failed to init socket.\n");
	return -1;
    }
    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0) {
	perror("[FAILED] : Failed to set socket opt.\n");
	return -1;
    }
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);
    address->sin_family = AF_INET;
    
    if (bind(server, (struct sockaddr*)address, sizeof(*address)) == -1) {
	perror("[FAILED] : Failed at binding.\n");
	close(server);
	return -1;
    }
    if (listen(server, 3) == -1) {
	perror("[FAILED] : Failed at listening ");
	close(server);
	return -1;
    }
    return server;
}
