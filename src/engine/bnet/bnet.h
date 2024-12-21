#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum NetworkType {
	BNET_CLIENT,
	BNET_SERVER
} NetworkType;

typedef enum BNetError {
	BNET_OK = 0,
	BNET_ERROR_SOCKET_FAILED,
	BNET_ERROR_BIND_FAILED,
} BNetError;

typedef struct BNetHandler {
	const NetworkType type;
	int socket;
} BNetHandler;

BNetError bnet_init(BNetHandler* handler, NetworkType type, const char* ip, int port);