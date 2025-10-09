#include "defs.h"
#include "ft_ping.h"

void ping(state_t *state)
{
	(void)state;
	while (1)
	{
		if (checkSIGINT())
			break;
	}
}
