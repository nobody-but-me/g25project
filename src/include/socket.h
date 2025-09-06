
#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <arpa/inet.h>
#define PORT 4242

int init_server(struct sockaddr_in* address);

#endif//SOCKET_UTILS_H
