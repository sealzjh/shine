#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define SERVER_ROOT "/home/www/test"
#define SERVER_PORT 8000
#define BACKLOG 10
#define MAX_FLUSH 128
