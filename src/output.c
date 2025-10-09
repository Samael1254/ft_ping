#include <stdio.h>

void error(const char *type, const char *msg)
{
	printf("ping: %s", type);
	if (msg)
		printf(": msg");
	printf("\n");
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
	printf("ping: invalid option -- '%c'\n", option);
	printf("Try 'ping --help' for more information.\n");
}

void unrecognizedOption(const char *option)
{
	printf("ping: unrecognized option '--%s'\n", option);
	printf("Try 'ping --help' for more information.\n");
}
