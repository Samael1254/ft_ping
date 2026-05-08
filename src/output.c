#include "ft_ping.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

void statistics(const char *hostname, uint32_t packetsSent, uint32_t packetsReceived, float *timeStats,
                uint32_t dataSize)
{
	timeStats[0] /= (float)packetsSent;
	timeStats[3] = sqrtf((timeStats[3] / (float)packetsSent) - timeStats[0] * timeStats[0]);
	uint32_t packetLoss = 100 - (int)roundf((float)packetsReceived / (float)packetsSent * 100);

	printf("--- %s ping statistics ---\n", hostname);
	printf("%u packets transmitted. %u packets received, %u%% packet loss\n", packetsSent, packetsReceived, packetLoss);
	if (dataSize >= sizeof(struct timeval))
		printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
		       timeStats[0],
		       timeStats[1],
		       timeStats[2],
		       timeStats[3]);
}

void receivedPacket(uint16_t packetSize, const char *ipstr, uint32_t seq, uint32_t ttl, float time, uint32_t dataSize)
{
	printf("%u bytes from %s: icmp_seq=%u ttl=%u", packetSize, ipstr, seq, ttl);
	if (dataSize >= sizeof(struct timeval))
		printf(" time=%.3f ms", time);
	printf("\n");
}

void help()
{
	printf("Usage: ping [OPTION...] HOST ...\n");
	printf("Send ICMP ECHO_REQUEST packets to network hosts.\n");
	printf("\n Options valid for all request types:\n");
	printf("  -c, --count=NUMBER         stop after sending NUMBER packets\n");
	printf("  -v, --verbose              verbose output\n");
	printf("\n Options valid for --echo requests:\n");
	printf("  -?, --help                 give this help list\n");
}
