#include "defs.h"
#include "ft_ping.h"
#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static void checkStrIsNumber(const char *str)
{
	size_t j;

	if (!str[0])
		return;
	j = 0;
	if ((str[0] == '-' || str[0] == '+') && str[1])
		j = 1;
	while (str[j])
	{
		if (!isdigit(str[j]))
			invalidValue(str, str + j);
		j++;
	}
}

static uint8_t getOptionId(const char option)
{
	const char validOptions[VALID_OPTIONS_NB] = {'v', '?', 'c', 'q', 'w'};
	for (uint8_t i = 0; i < VALID_OPTIONS_NB; i++)
		if (option == validOptions[i])
			return i;
	exit(64);
}

int getOptions(int argc, char **argv, option_t *options)
{
	int                 c = 0;
	const char          shortopts[] = ":v?qc:w:i:";
	const struct option longopts[] = {{"verbose", no_argument, NULL, 'v'},
	                                  {"help", no_argument, NULL, '?'},
	                                  {"count", required_argument, NULL, 'c'},
	                                  {"quiet", no_argument, NULL, 'q'},
	                                  {"timeout", required_argument, NULL, 'w'},
	                                  {"interval", required_argument, NULL, 'i'},
	                                  {}};
	opterr = 0; // Do not let getopt print error messages

	printf("TESTETSTETST1\n");
	while (true)
	{
		int longindex = 0;
		c = getopt_long(argc, argv, shortopts, longopts, &longindex);
		if (c == -1)
		{
			printf("TESTETSTETST2\n");
			break;
		}
		if (c == '?')
		{
			if (strcmp(argv[optind - 1], "-?") != 0 && strcmp(argv[optind - 1], "--help") != 0)
			{
				if (optopt == 0)
					unrecognizedOption(argv[optind - 1]);
				invalidOption((char)optopt);
			}
		}
		if (c == ':')
		{
			if (strlen(argv[optind - 1]) > 1)
				argRequiredLong(argv[optind - 1]);
			argRequired((char)optopt);
		}
		if (c == 'c')
			checkStrIsNumber(optarg);
		int       id = getOptionId((char)c);
		option_t *option = &options[id];
		option->isActive = true;
		if (optarg)
		{
			option->hasParam = true;
			option->param = strdup(optarg);
		}
	}
	printf("TESTETSTETST\n");
	return optind;
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
		hosts[i++].name = *args++;
	return hosts;
}
