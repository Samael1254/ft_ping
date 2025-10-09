#include "ft_ping.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

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
	char **hosts = getHosts(argv + 1);
	if (!hosts)
		fatalError("malloc failed", "in getHosts");
	if (options[Help])
	{
		help();
		return EXIT_SUCCESS;
	}
}
