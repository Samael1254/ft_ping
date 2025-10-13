#include "defs.h"
#include "ft_ping.h"
#include <arpa/inet.h>
#include <bits/types/struct_timeval.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

static void connectToHost(host_t *host, state_t *state)
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
	int              status = getaddrinfo(host->name, NULL, &hints, &info);
	if (status != 0)
		fatalError("getaddrinfo failed in establishConnection", gai_strerror(status), state, true);

	struct addrinfo *iter = info;
	while (iter)
	{
		host->addr = *(struct sockaddr_in *)iter->ai_addr;
		inet_ntop(AF_INET, &host->addr.sin_addr, host->ipstr, sizeof(host->ipstr));
		iter = iter->ai_next;
	}

	freeaddrinfo(info);
	printf("PING %s (%s): %d data bytes\n", host->name, host->ipstr, 56);
}

static void setupSocket(state_t *state)
{
	state->socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (state->socket == -1)
		fatalError("socket failed in setupSocket", strerror(errno), state, false);
}

static float roundTripTime(const char *packet, state_t *state)
{
	struct timeval sendTime = *(struct timeval *)(packet + sizeof(struct iphdr) + sizeof(struct icmphdr));
	struct timeval recvTime;
	if (gettimeofday(&recvTime, NULL) == -1)
		fatalError("gettimeofday failed in roundTripTime", strerror(errno), state, true);
	return (float)(recvTime.tv_sec - sendTime.tv_sec) * 1000 + (float)(recvTime.tv_usec - sendTime.tv_usec) / 1000;
}

static void updateTimeStats(float *timeStats, float rtt)
{
	timeStats[0] += rtt;
	if (timeStats[1] == 0 || rtt < timeStats[1])
		timeStats[1] = rtt;
	if (rtt > timeStats[2])
		timeStats[2] = rtt;
	timeStats[3] += rtt * rtt;
}

static void receiveResponse(char *buf, state_t *state, host_t *host, uint32_t *received, uint32_t seq, float *timeStats)
{
	ssize_t bytesNb = recvfrom(state->socket, buf, BUFSIZE, 0, NULL, NULL);
	if (bytesNb == -1)
		fatalError("recvfrom failed in ping", strerror(errno), state, true);
	(*received)++;
	float rtt = roundTripTime(buf, state);
	receivedPacket(bytesNb - sizeof(struct iphdr), host->ipstr, seq, 0, rtt);
	updateTimeStats(timeStats, rtt);
}

static void sendPacket(struct timeval *lastSend, state_t *state, host_t *host, uint32_t *seq, char *packet)
{
	createEchoRequest(packet);
	if (sendto(state->socket, packet, sizeof(struct icmphdr) + DEFAULT_DATA_SIZE, 0, (struct sockaddr *)&host->addr,
	           sizeof(host->addr)) == -1)
		fatalError("sendto failed in ping", strerror(errno), state, true);
	(*seq)++;
	if (gettimeofday(lastSend, NULL) == -1)
		fatalError("gettimeofday failed in sendPacket", strerror(errno), state, true);
}

void ping(state_t *state)
{
	fd_set         rfds;
	struct timeval tv = {
	    .tv_sec = 0,
	    .tv_usec = 0,
	};
	char  packet[sizeof(struct icmphdr) + DEFAULT_DATA_SIZE];
	float timeStats[4] = {0};

	setupSocket(state);
	for (uint8_t i = 0; i < state->hostsCount; i++)
	{
		struct timeval lastSend = {
		    .tv_sec = 0,
		    .tv_usec = 0,
		};
		host_t  *host = &state->hosts[i];
		uint32_t seq = 0;
		uint32_t received = 0;

		connectToHost(host, state);
		while (1)
		{
			FD_ZERO(&rfds);
			FD_SET(state->socket, &rfds);
			select(state->socket + 1, &rfds, NULL, NULL, &tv);
			char buf[BUFSIZE] = {0};
			if (FD_ISSET(state->socket, &rfds))
				receiveResponse(buf, state, host, &received, seq, timeStats);

			struct timeval timer;
			if (gettimeofday(&timer, NULL) == -1)
				fatalError("gettimeofday failed in ping", strerror(errno), state, true);
			if (timer.tv_sec - lastSend.tv_sec >= 1 && timer.tv_usec > lastSend.tv_usec)
				sendPacket(&lastSend, state, host, &seq, packet);

			if (checkSIGINT(state))
				break;
		}
		statistics(host->name, seq, received, timeStats);
	}
	close(state->socket);
}
