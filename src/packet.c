#include "defs.h"
#include "ft_ping.h"
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

static uint16_t checksum(const void *buf, uint16_t size)
{
	uint32_t        sum = 0;
	int             nleft = size;
	const uint16_t *data = buf;

	while (nleft > 1)
	{
		sum += *data++;
		nleft -= 2;
	}
	if (nleft == 1)
		sum += *(const uint8_t *)data;
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return ~sum;
}

static void fillData(char *start, uint16_t size, const char *pattern)
{
	// if (!pattern)
	// {
	(void)pattern;
	char c = 'a';
	for (uint16_t i = 0; i < size; i++)
		start[i] = c++;
	return;
	// }
}

void createEchoRequest(char *packet, uint32_t dataSize, const char *pattern)
{
	static uint16_t sequence;
	struct icmphdr *header = (struct icmphdr *)packet;
	header->type = ICMP_ECHO;
	header->code = 0;
	header->un.echo.id = htons(getpid());
	header->un.echo.sequence = htons(sequence++);
	header->checksum = 0;

	char *data = packet + sizeof(struct icmphdr);
	if (dataSize >= sizeof(struct timeval))
	{
		struct timeval *tv = (struct timeval *)data;
		gettimeofday(tv, NULL);
		fillData(data + sizeof(struct timeval), dataSize - sizeof(struct timeval), pattern);
	}
	else
		fillData(data, dataSize, pattern);

	header->checksum = checksum(packet, sizeof(struct icmphdr) + DEFAULT_DATA_SIZE);
}

bool parseEchoReply(const char *packet, size_t bytesRead, uint8_t *ttl, float *rtt)
{
	const struct iphdr *ipHeader = (struct iphdr *)packet;
	size_t              ipHeaderLen = (size_t)ipHeader->ihl * 4;
	if (bytesRead < ipHeaderLen + sizeof(struct icmphdr)) // Packet is too small for ip and icmp headers
		return false;

	const struct icmphdr *icmpHeader = (struct icmphdr *)(packet + ipHeaderLen);
	if (icmpHeader->type != ICMP_ECHOREPLY) // Packet is not an ICMP reply
		return false;
	if (ntohs(icmpHeader->un.echo.id) != getpid()) // Packet does not belong to this process
		return false;

	struct timeval sendTime = *(struct timeval *)(packet + sizeof(struct iphdr) + sizeof(struct icmphdr));
	struct timeval recvTime;
	gettimeofday(&recvTime, NULL);
	*rtt = (float)(recvTime.tv_sec - sendTime.tv_sec) * 1000 + (float)(recvTime.tv_usec - sendTime.tv_usec) / 1000;

	*ttl = ipHeader->ttl;

	return true;
}
