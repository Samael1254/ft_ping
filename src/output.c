#include <stdio.h>
#include <stdlib.h>

void error(const char *type, const char *msg)
{
	fprintf(stderr, "ping: %s", type);
	if (msg)
		fprintf(stderr, ": %s", msg);
	fprintf(stderr, "\n");
	fprintf(stderr, "Try 'ping --help' for more information.\n");
}

void fatalError(const char *type, const char *msg)
{
	fprintf(stderr, "ping: fatal error: %s", type);
	if (msg)
		fprintf(stderr, ": %s", msg);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
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
