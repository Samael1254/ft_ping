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

static void checkStrIsHex(const char *str)
{
	size_t j;

	if (!str[0])
		return;
	j = 0;
	while (str[j])
	{
		if (!isdigit(str[j]) && !(str[j] >= 'A' && str[j] <= 'F') && !(str[j] >= 'a' && str[j] <= 'f'))
			errorPattern(str + j);
		j++;
	}
}

static uint8_t getOptionId(const char option)
{
	const char validOptions[VALID_OPTIONS_NB] = {'v', '?', 'c', 'q', 'w', 'i', 's', 'p'};
	for (uint8_t i = 0; i < VALID_OPTIONS_NB; i++)
		if (option == validOptions[i])
			return i;
	exit(64);
}

int getOptions(int argc, char **argv, option_t *options)
{
	int                 c = 0;
	const char          shortopts[] = ":v?qc:w:i:s:p:";
	const struct option longopts[] = {
	    {"verbose", no_argument, NULL, 'v'},
	    {"help", no_argument, NULL, '?'},
	    {"count", required_argument, NULL, 'c'},
	    {"quiet", no_argument, NULL, 'q'},
	    {"timeout", required_argument, NULL, 'w'},
	    {"interval", required_argument, NULL, 'i'},
	    {"size", required_argument, NULL, 's'},
	    {"pattern", required_argument, NULL, 'p'},
	    {},
	};
	opterr = 0; // Do not let getopt print error messages

	while (true)
	{
		c = getopt_long(argc, argv, shortopts, longopts, NULL);
		if (c == -1)
			break;
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
		if (c == 'c' || c == 't' || c == 'i' || c == 's')
			checkStrIsNumber(optarg);
		if (c == 'p')
			checkStrIsHex(optarg);
		int       id = getOptionId((char)c);
		option_t *option = &options[id];
		option->isActive = true;
		if (optarg)
		{
			option->hasParam = true;
			option->param = strdup(optarg);
		}
	}
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
