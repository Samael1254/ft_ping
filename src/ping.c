#include "defs.h"
#include "ft_ping.h"
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

static void establishConnection(const char *host)
{
	socket(AF_INET, SOCK_DGRAM, 1);
	char ipaddr[16] = "127.0.0.1";
	printf("PING %s (%s): %d data bytes\n", host, ipaddr, 56);
}

void ping(state_t *state)
{
	establishConnection(state->hosts[0]);
	while (1)
	{
		if (checkSIGINT())
			break;
		sleep(1);
	}
}
