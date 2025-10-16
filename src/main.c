#include "defs.h"
#include "ft_ping.h"
#include <stdbool.h>
#include <stdint.h>
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
	getOptions(argc, argv, state->options);
	state->hostsCount = countHosts(argv + 1);
	state->hosts = getHosts(argv + 1, state->hostsCount);
	if (!state->hosts)
		fatalError("malloc failed in getHosts", state, false);
}

int main(int argc, char **argv)
{
	if (argc == 1)
		argError("missing host operand");

	state_t state = {};
	initState(&state, argc, argv);

	setSignalHandlers(&state);
	if (state.options[Help].isActive)
	{
		help();
		return EXIT_SUCCESS;
	}
	ping(&state);
	freeState(&state);
}
