#include "defs.h"
#include "ft_ping.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

static void freeHost(host_t *host)
{
	(void)host;
}

void freeState(state_t *state)
{
	if (!state)
		return;
	for (uint8_t i = 0; i < state->hostsCount; i++)
		freeHost(&state->hosts[i]);
	if (state->hosts)
		free(state->hosts);
}

void initState(state_t *state, char **argv)
{
	state->hostsCount = countHosts(argv + 1);
	state->hosts = getHosts(argv + 1, state->hostsCount);
	if (!state->hosts)
		fatalError("malloc failed in getHosts", state, false);
}

int main(int argc, char **argv)
{
	if (argc == 1)
		argError("missing host operand");

	bool options[VALID_OPTIONS_NB];
	memset(options, 0, VALID_OPTIONS_NB * sizeof(bool));
	getOptions(argv + 1, options);

	state_t state = {};
	initState(&state, argv);

	setSignalHandlers(&state);
	if (options[Help])
	{
		help();
		return EXIT_SUCCESS;
	}
	ping(&state);
	freeState(&state);
}
