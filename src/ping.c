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
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

static void connectToHost(host_t *host, state_t *state)
{
	struct addrinfo hints = {
	    .ai_family = AF_INET,
	    .ai_socktype = SOCK_RAW,
	    .ai_flags = AI_PASSIVE,
	    .ai_protocol = IPPROTO_ICMP,
	    .ai_canonname = NULL,
	    .ai_addr = NULL,
	    .ai_next = NULL,
	};

	struct addrinfo *info;
	int              status = getaddrinfo(host->name, NULL, &hints, &info);
	if (status != 0)
		error("unknown host", state, true);

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
		fatalError("socket failed in setupSocket", state, false);
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

static void receivePacket(char *buf, state_t *state, host_t *host, uint32_t *receivedCount, uint32_t seq,
                          float *timeStats)
{
	ssize_t bytesNb = recvfrom(state->socket, buf, BUFSIZE, 0, NULL, NULL);
	if (bytesNb == -1)
		fatalError("recvfrom failed in ping", state, true);

	uint8_t ttl;
	float   rtt;
	if (!parseEchoReply(buf, (size_t)bytesNb, &ttl, &rtt))
		return;
	(*receivedCount)++;
	if (!state->options[Quiet].isActive)
		receivedPacket(bytesNb - sizeof(struct iphdr), host->ipstr, seq - 1, ttl, rtt);
	updateTimeStats(timeStats, rtt);
}

static void sendPacket(struct timeval *lastSend, state_t *state, host_t *host, uint32_t *seq, char *packet)
{
	createEchoRequest(packet);
	if (sendto(state->socket,
	           packet,
	           sizeof(struct icmphdr) + DEFAULT_DATA_SIZE,
	           0,
	           (struct sockaddr *)&host->addr,
	           sizeof(host->addr)) == -1)
		fatalError("sendto failed in ping", state, true);
	(*seq)++;
	gettimeofday(lastSend, NULL);
}

void ping(state_t *state)
{
	fd_set rfds;
	char   packet[sizeof(struct icmphdr) + DEFAULT_DATA_SIZE];
	float  timeStats[4] = {0};

	setupSocket(state);
	for (uint8_t i = 0; i < state->hostsCount; i++)
	{
		struct timeval globalTime;
		gettimeofday(&globalTime, NULL);
		struct timeval lastSend = {
		    .tv_sec = 0,
		    .tv_usec = 0,
		};
		host_t  *host = &state->hosts[i];
		uint32_t seq = 0;
		uint32_t receivedCount = 0;
		int32_t  maxCount = -1;
		if (state->options[Count].isActive)
			maxCount = atoi(state->options[Count].param);
		int32_t processTimeout = -1;
		if (state->options[Timeout].isActive)
			processTimeout = atoi(state->options[Timeout].param);

		connectToHost(host, state);

		while (1 && (maxCount == -1 || receivedCount < (uint32_t)maxCount))
		{
			struct timeval timeout;
			struct timeval now;
			gettimeofday(&now, NULL);
			long timeLeft = 1000 - ((now.tv_sec - lastSend.tv_sec) * 1000 + (now.tv_usec - lastSend.tv_usec) / 1000);
			if (timeLeft <= 0)
			{
				sendPacket(&lastSend, state, host, &seq, packet);
				timeLeft = 1000;
			}
			timeout.tv_sec = timeLeft / 1000;
			timeout.tv_usec = (timeLeft % 1000) * 1000;

			FD_ZERO(&rfds);
			FD_SET(state->socket, &rfds);
			int ret = select(state->socket + 1, &rfds, NULL, NULL, &timeout);
			if (ret == -1)
			{
				if (checkSIGINT(state))
					break;
				continue;
			}

			if (FD_ISSET(state->socket, &rfds))
			{
				char buf[BUFSIZE] = {0};
				receivePacket(buf, state, host, &receivedCount, seq, timeStats);
			}

			if (checkSIGINT(state))
				break;
		}
		statistics(host->name, seq, receivedCount, timeStats);
	}
	close(state->socket);
}
