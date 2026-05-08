#include "defs.h"
#include "ft_ping.h"
#include <arpa/inet.h>
#include <bits/types/struct_timeval.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

static void connectToHost(host_t *host, state_t *state, uint32_t dataSize)
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
	int pid = getpid();
	printf("PING %s (%s): %u data bytes", host->name, host->ipstr, dataSize);
	if (state->options[Verbose].isActive)
		printf(", id 0x%04x = %d", pid, pid);
	printf("\n");
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
                          float *timeStats, uint32_t dataSize)
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
		receivedPacket(bytesNb - sizeof(struct iphdr), host->ipstr, seq - 1, ttl, rtt, dataSize);
	if (dataSize >= sizeof(struct timeval))
		updateTimeStats(timeStats, rtt);
}

static void sendPacket(state_t *state, host_t *host, uint32_t *seq, char *packet, uint32_t dataSize,
                       const char *pattern)
{
	createEchoRequest(packet, dataSize, pattern);
	if (sendto(state->socket,
	           packet,
	           sizeof(struct icmphdr) + dataSize,
	           0,
	           (struct sockaddr *)&host->addr,
	           sizeof(host->addr)) == -1)
		fatalError("sendto failed in ping", state, true);
	(*seq)++;
}

void ping(state_t *state)
{
	fd_set rfds;
	float  timeStats[4] = {0};

	setupSocket(state);
	for (uint8_t i = 0; i < state->hostsCount; i++)
	{
		struct timeval startTime;
		gettimeofday(&startTime, NULL);
		struct timeval lastSend = {
		    .tv_sec = 0,
		    .tv_usec = 0,
		};
		host_t  *host = &state->hosts[i];
		uint32_t seq = 0;
		uint32_t receivedCount = 0;

		int32_t     maxCount = state->options[Count].isActive ? atoi(state->options[Count].param) : -1;
		int32_t     processTimeout = state->options[Timeout].isActive ? atoi(state->options[Timeout].param) : -1;
		int32_t     interval = state->options[Interval].isActive ? atoi(state->options[Interval].param) : 1;
		uint16_t    dataSize = state->options[Size].isActive ? atoi(state->options[Size].param) : DEFAULT_DATA_SIZE;
		const char *pattern = state->options[Pattern].isActive ? state->options[Pattern].param : NULL;

		char *packet = calloc(sizeof(struct icmphdr) + dataSize, sizeof(char));
		if (!packet)
			fatalError("malloc failed in ping", state, true);

		connectToHost(host, state, dataSize);

		while (1 && (maxCount == -1 || receivedCount < (uint32_t)maxCount))
		{
			struct timeval selectTimeout;
			struct timeval now;
			gettimeofday(&now, NULL);
			long timeLeft =
			    1000L * interval - ((now.tv_sec - lastSend.tv_sec) * 1000 + (now.tv_usec - lastSend.tv_usec) / 1000);
			if (timeLeft <= 0)
			{
				sendPacket(state, host, &seq, packet, dataSize, pattern);
				gettimeofday(&lastSend, NULL);
				timeLeft = 1000L * interval;
			}
			selectTimeout.tv_sec = timeLeft / 1000;
			selectTimeout.tv_usec = (timeLeft % 1000) * 1000;
			long totalTime = now.tv_sec - startTime.tv_sec;
			if (processTimeout > -1 && totalTime >= processTimeout)
				break;

			FD_ZERO(&rfds);
			FD_SET(state->socket, &rfds);
			int ret = select(state->socket + 1, &rfds, NULL, NULL, &selectTimeout);
			if (ret == -1)
			{
				if (checkSIGINT(state))
					break;
				continue;
			}

			if (FD_ISSET(state->socket, &rfds))
			{
				char buf[BUFSIZE] = {0};
				receivePacket(buf, state, host, &receivedCount, seq, timeStats, dataSize);
			}

			if (checkSIGINT(state))
				break;
		}
		free(packet);
		statistics(host->name, seq, receivedCount, timeStats, dataSize);
	}
	close(state->socket);
}
