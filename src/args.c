#include "defs.h"
#include "ft_ping.h"
#include "getopt.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static uint8_t getOptionId(const char option)
{
	const char validOptions[VALID_OPTIONS_NB] = {'v', '?', 'c'};
	for (uint8_t i = 0; i < VALID_OPTIONS_NB; i++)
		if (option == validOptions[i])
			return i;
	exit(64);
}

void getOptions(int argc, char **argv, option_t *options)
{
	int                 c = 0;
	const char          shortopts[] = ":v?c:";
	const struct option longopts[] = {{"verbose", no_argument, NULL, 'v'},
	                                  {"help", no_argument, NULL, '?'},
	                                  {"count", required_argument, NULL, 'c'},
	                                  {}};
	opterr = 0;

	while (true)
	{
		int longindex = 0;
		c = getopt_long(argc, argv, shortopts, longopts, &longindex);
		if (c == -1)
			break;
		printf("optopt: %c, c: %c, arg: %s\n", optopt, c, optarg);
		if (c == '?')
		{
			if (optopt == 0)
				unrecognizedOption(argv[optind - 1]);
			invalidOption((char)optopt);
		}
		if (c == ':')
		{
			if (strlen(argv[optind - 1]) > 1)
				argRequiredLong(argv[optind - 1]);
			argRequired((char)optopt);
		}
		int       id = getOptionId((char)c);
		option_t *option = &options[id];
		option->isActive = true;
		if (optarg)
		{
			option->hasParam = true;
			option->param = strdup(optarg);
		}
	}
}

uint8_t countHosts(char **args)
{
	uint8_t count = 0;

	while (*args)
	{
		if ((*args)[0] != '-')
			count++;
		args++;
	}
	return count;
}

host_t *getHosts(char **args, uint8_t hostsCount)
{
	if (hostsCount == 0)
		argError("missing host operand");
	host_t *hosts = (host_t *)malloc(hostsCount * sizeof(host_t));
	if (!hosts)
		return NULL;

	uint8_t i = 0;
	while (*args)
	{
		if ((*args)[0] != '-')
			hosts[i++].name = *args;
		args++;
	}
	return hosts;
}
