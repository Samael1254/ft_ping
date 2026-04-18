#include "defs.h"
#include <ft_ping.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>

volatile sig_atomic_t g_stop_ping = 0;

static void interrupt(int signal)
{
	(void)signal;
	g_stop_ping = 1;
}

bool checkSIGINT(state_t *state)
{
	(void)state;
	return g_stop_ping == 1;
}

void setSignalHandlers(state_t *state)
{
	struct sigaction sa = {0};
	sa.sa_handler = interrupt;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGINT, &sa, NULL) == -1)
		fatalError("sigaction failed in setSignalHandlers", state, false);
}
