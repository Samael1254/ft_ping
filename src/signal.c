#include <errno.h>
#include <ft_ping.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>

static void interrupt(int signal)
{
	(void)signal;
}

bool checkSIGINT()
{
	sigset_t set;
	sigemptyset(&set);
	if (sigpending(&set) == -1)
		fatalError("sigpending failed in ping", strerror(errno));
	int isMember = sigismember(&set, SIGINT);
	if (isMember == -1)
		fatalError("sigismember failed in ping", strerror(errno));
	return (bool)isMember;
}

void setSignalHandlers()
{
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigprocmask(SIG_BLOCK, &mask, NULL);

	struct sigaction sa = {0};
	sa.sa_handler = interrupt;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL) == -1)
		fatalError("sigaction failed in setSignalHandlers", strerror(errno));
}
