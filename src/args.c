#include "defs.h"
#include "ft_ping.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static uint8_t getOptionId(const char option)
{
	const char validOptions[VALID_OPTIONS_NB] = {'v', '?'};
	for (uint8_t i = 0; i < VALID_OPTIONS_NB; i++)
		if (option == validOptions[i])
			return i;
	invalidOption(option);
	exit(64);
}

static uint8_t getLongOptionId(const char *option)
{
	const char *validLongOptions[VALID_OPTIONS_NB] = {"verbose", "help"};
	for (uint8_t i = 0; i < VALID_OPTIONS_NB; i++)
		if (strcmp(option, validLongOptions[i]) == 0)
			return i;
	unrecognizedOption(option);
	exit(64);
}

void getOptions(char **args, bool *options)
{
	while (*args)
	{
		const char *arg = *args;
		if (arg[0] != '-')
		{
			args++;
			continue;
		}
		if (arg[1] == '-')
		{
			options[getLongOptionId(arg + 2)] = true;
			args++;
			continue;
		}
		for (unsigned int j = 1; j < strlen(arg); j++)
			options[getOptionId(arg[j])] = true;
		args++;
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
