#include "ft_ping.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void error(const char *type, const char *msg)
{
	fprintf(stderr, "ping: %s", type);
	if (msg)
		fprintf(stderr, ": %s", msg);
	fprintf(stderr, "\n");
	fprintf(stderr, "Try 'ping --help' for more information.\n");
}

void fatalError(const char *type, const char *msg, state_t *state, bool closeSocket)
{
	fprintf(stderr, "ping: fatal error: %s", type);
	if (msg)
		fprintf(stderr, ": %s", msg);
	fprintf(stderr, "\n");

	if (closeSocket)
		close(state->socket);
	freeState(state);
	exit(EXIT_FAILURE);
}

void statistics(const char *hostname, uint32_t packetsSent, uint32_t packetsReceived)
{
	uint32_t packetLoss = 100 - (int)roundf((float)packetsReceived / (float)packetsSent * 100);
	printf("--- %s ping statistics ---\n", hostname);
	printf("%u packets transmitted. %u packets received, %u%% packet loss\n", packetsSent, packetsReceived, packetLoss);
	printf("round-trip min/avg/max/stddev = \n");
}

void receivedPacket(uint16_t packetSize, const char *ipstr, uint32_t seq, uint32_t ttl, float time)
{
	printf("%u bytes from %s: icmp_seq=%u ttl=%u time=%f ms\n", packetSize, ipstr, seq, ttl, time);
}

void help(void)
{
	printf("Usage: ping [OPTION...] HOST ...\n");
	printf("Send ICMP ECHO_REQUEST packets to network hosts.\n");
	printf("\n Options valid for all request types:\n");
	printf("  -v, --verbose              verbose output\n");
	printf("\n Options valid for --echo requests:\n");
	printf("  -?, --help                 give this help list\n");
}

void invalidOption(char option)
{
	fprintf(stderr, "ping: invalid option -- '%c'\n", option);
	fprintf(stderr, "Try 'ping --help' for more information.\n");
}

void unrecognizedOption(const char *option)
{
	fprintf(stderr, "ping: unrecognized option '--%s'\n", option);
	fprintf(stderr, "Try 'ping --help' for more information.\n");
}
