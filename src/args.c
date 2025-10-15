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
	const char validOptions[VALID_OPTIONS_NB] = {'v', '?'};
	for (uint8_t i = 0; i < VALID_OPTIONS_NB; i++)
		if (option == validOptions[i])
			return i;
	exit(64);
}

void getOptions(int argc, char **argv, option_t *options)
{
	int        c = 0;
	const char shortopts[] = "v?";
	int        val = 0;

	const struct option longopts[] = {{"help", no_argument, &val, '?'}, {"verbose", no_argument, &val, 'v'}, {}};
	opterr = 0;

	while (true)
	{
		int longindex = 0;
		c = getopt_long(argc, argv, shortopts, longopts, &longindex);
		if (c == -1)
			break;
		if (c == '?')
		{
			if (optopt == 0)
				unrecognizedOption(argv[optind - 1]);
			else
				invalidOption((char)optopt);
			exit(64);
		}
		int       id = c == 0 ? getOptionId((char)val) : getOptionId((char)c);
		option_t *option = &options[id];
		option->isActive = true;

		// printf("optopt = %c, result: %c\n", optopt, c);
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
