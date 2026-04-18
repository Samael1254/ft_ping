#include "defs.h"
#include "ft_ping.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

static void freeHost(host_t *host)
{
	(void)host;
}

void freeState(state_t *state)
{
	for (uint8_t i = 0; i < state->hostsCount; i++)
		freeHost(&state->hosts[i]);
	for (uint8_t i = 0; i < VALID_OPTIONS_NB; i++)
		if (state->options[i].hasParam)
			free(state->options[i].param);
	if (state->hosts)
		free(state->hosts);
}

void initState(state_t *state, int argc, char **argv)
{
	int nextIndex = getOptions(argc, argv, state->options);
	if (state->options[Help].isActive)
	{
		help();
		exit(EXIT_SUCCESS);
	}
	state->hostsCount = argc - nextIndex;
	state->hosts = getHosts(argv + nextIndex, state->hostsCount);
	if (!state->hosts)
		fatalError("malloc failed in getHosts", state, false);
}

int main(int argc, char **argv)
{
	state_t state = {};
	initState(&state, argc, argv);

	setSignalHandlers(&state);
	ping(&state);
	freeState(&state);
	return EXIT_SUCCESS;
}
