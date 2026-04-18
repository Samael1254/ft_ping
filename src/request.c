#include "defs.h"
#include "ft_ping.h"
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <unistd.h>

static uint16_t checksum(void *buf, uint16_t size)
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

static void fillData(char *start, uint16_t size)
{
	char c = 'a';
	for (uint16_t i = 0; i < size; i++)
		start[i] = c++;
}

void createEchoRequest(char *packet)
{
	static uint16_t sequence;
	struct icmphdr *request = (struct icmphdr *)packet;
	request->type = ICMP_ECHO;
	request->code = 0;
	request->un.echo.id = htons(getpid());
	request->un.echo.sequence = htons(sequence++);
	request->checksum = 0;

	uint16_t dataSize = DEFAULT_DATA_SIZE;
	char    *data = packet + sizeof(struct icmphdr);
	if (dataSize >= sizeof(struct timeval))
	{
		struct timeval *tv = (struct timeval *)data;
		gettimeofday(tv, NULL);
		fillData(data + sizeof(struct timeval), dataSize - sizeof(struct timeval));
	}
	else
		fillData(data, dataSize);

	uint16_t packetLen = sizeof(struct icmphdr) + DEFAULT_DATA_SIZE;
	request->checksum = checksum(packet, packetLen);
}
