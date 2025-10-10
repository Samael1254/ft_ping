#include "defs.h"
#include "ft_ping.h"
#include <arpa/inet.h>
#include <bits/types/struct_timeval.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

static uint16_t checksum(void *buf, uint32_t size)
{
	(void)buf;
	(void)size;
	return 0;
}

static void fillData(char *start, uint16_t size)
{
	char c = 'a';
	for (uint16_t i = 0; i < size; i++, c++)
		start[i] = c++;
}

static void createEchoRequest(char *buf)
{
	static uint16_t sequence;
	struct icmphdr *request = (struct icmphdr *)buf;
	request->type = ICMP_ECHO;
	request->code = 0;
	request->un.echo.id = getpid();
	request->un.echo.sequence = sequence++;
	request->checksum = 0;

	uint16_t dataSize = DEFAULT_DATA_SIZE;
	char    *data = buf + sizeof(struct icmphdr);
	if (dataSize >= sizeof(struct timeval))
	{
		struct timeval *tv = (struct timeval *)data;
		gettimeofday(tv, NULL);
		fillData(data + sizeof(struct timeval), dataSize - sizeof(struct timeval));
	}
	else
		fillData(data, dataSize);

	request->checksum = checksum(buf, BUFSIZE);
}

static void establishConnection(host_t *host)
{
	struct addrinfo hints = {
	    .ai_family = AF_INET,
	    .ai_socktype = SOCK_RAW,
	    .ai_flags = AI_PASSIVE,
	    .ai_protocol = 1,
	    .ai_canonname = NULL,
	    .ai_addr = NULL,
	    .ai_next = NULL,
	};

	struct addrinfo *info;

	int status = getaddrinfo(host->name, NULL, &hints, &info);
	if (status != 0)
		fatalError("getaddrinfo failed in establishConnection", gai_strerror(status));

	struct addrinfo *iter = info;
	while (iter)
	{
		host->addr = *(struct sockaddr_in *)iter->ai_addr;
		inet_ntop(AF_INET, &host->addr.sin_addr, host->ipstr, sizeof(host->ipstr));
		iter = iter->ai_next;
	}

	freeaddrinfo(info);
	printf("PING %s (%s): %d data bytes\n", host->name, host->ipstr, 56);
	// char hostname[256];
	// char service[256];
	// getnameinfo((struct sockaddr *)&host->addr, sizeof(host->addr), hostname, 256, service, 256, 0);
	// printf("hostname: %s, service: %s\n", hostname, service);
	// const char ipaddr[16] = "127.0.0.1";
	// {
	// }
}

// static void setupHost(host_t *host) {}

void ping(state_t *state)
{
	state->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
	for (uint8_t i = 0; i < state->hostsCount; i++)
	{
		host_t host = state->hosts[i];
		establishConnection(&host);
		char buf[sizeof(struct iphdr) + sizeof(struct icmphdr) + DEFAULT_DATA_SIZE];
		while (1)
		{
			if (checkSIGINT())
				break;
			sendto(state->socket, buf, sizeof(buf), 0, (struct sockaddr *)&host.addr, sizeof(host.addr));
			sleep(1);
		}
	}
}
