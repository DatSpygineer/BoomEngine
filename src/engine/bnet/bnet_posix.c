#include "bnet.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

BNetError bnet_init(BNetHandler* handler, NetworkType type, const char* ip, int port) {
	struct sockaddr_in server_addr;
	handler->socket = socket(AF_INET, SOCK_STREAM, 0);
	if (handler->socket < 0) {
		return BNET_ERROR_SOCKET_FAILED;
	}

	return BNET_OK;
}