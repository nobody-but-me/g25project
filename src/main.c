

#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "./include/request_handler.h"
#include "./include/socket.h"

int main()
{
    struct sockaddr_in address;
    int server, socket;
    
    int addrlen = sizeof(address);
    
    server = init_server(&address);
    if (server == -1) return -1;
    printf("[INFO] : Server is running at http://localhost:4242 address.\n");
    while (1) {
	if ((socket = accept(server, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
	    perror("[FAILED] : Connection have not been accepted. \n");
	    continue;
	}
	handle_client(socket);
    }
    close(server);
    return 0;
}
