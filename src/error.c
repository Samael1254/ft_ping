#include "ft_ping.h"
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void error(const char *msg, state_t *state, bool closeSocket)
{
	fprintf(stderr, "ping: %s\n", msg);
	if (closeSocket)
		close(state->socket);
	freeState(state);
	exit(EXIT_FAILURE);
}

void argError(const char *msg)
{
	fprintf(stderr, "ping: %s\n", msg);
	fprintf(stderr, "Try 'ping --help' for more information.\n");
	exit(64);
}

void invalidOption(char option)
{
	fprintf(stderr, "ping: invalid option -- '%c'\n", option);
	fprintf(stderr, "Try 'ping --help' for more information.\n");
	exit(64);
}

void unrecognizedOption(const char *option)
{
	fprintf(stderr, "ping: unrecognized option '%s'\n", option);
	fprintf(stderr, "Try 'ping --help' for more information.\n");
	exit(64);
}

void argRequired(char option)
{
	fprintf(stderr, "ping: option requires an argument -- '%c'\n", option);
	fprintf(stderr, "Try 'ping --help' for more information.\n");
	exit(64);
}

void argRequiredLong(const char *option)
{
	fprintf(stderr, "ping: option '%s' requires an argument \n", option);
	fprintf(stderr, "Try 'ping --help' for more information.\n");
	exit(64);
}

void fatalError(const char *msg, state_t *state, bool closeSocket)
{
	fprintf(stderr, "ping: %s: %s\n", msg, strerror(errno));
	if (closeSocket)
		close(state->socket);
	freeState(state);
	exit(EXIT_FAILURE);
}
