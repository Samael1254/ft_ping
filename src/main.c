#include "ft_ping.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

int main(int argc, char **argv)
{
	(void)argc;
	if (argc == 1)
		return 1;
	bool options[VALID_OPTIONS_NB];
	memset(options, 0, VALID_OPTIONS_NB * sizeof(bool));
	getOptions(argv + 1, options);
	if (options[Help])
	{
		help();
		exit(EXIT_SUCCESS);
	}
}
