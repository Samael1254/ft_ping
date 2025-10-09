#include "defs.h"
#include "ft_ping.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

void freeState(state_t *state)
{
	if (state->hosts)
		free(state->hosts);
}

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		error("missing host operand", NULL);
		return EXIT_FAILURE;
	}
	bool options[VALID_OPTIONS_NB];
	memset(options, 0, VALID_OPTIONS_NB * sizeof(bool));
	getOptions(argv + 1, options);
	state_t state;
	memset(&state, 0, sizeof(state_t));
	state.hosts = getHosts(argv + 1);
	if (!state.hosts)
		fatalError("malloc failed", "in getHosts");
	setSignalHandlers();
	if (options[Help])
	{
		help();
		return EXIT_SUCCESS;
	}
	ping(&state);
	freeState(&state);
}
