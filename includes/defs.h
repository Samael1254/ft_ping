#ifndef DEFS_H
#define DEFS_H

#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>

#define VALID_OPTIONS_NB  2
#define BUFSIZE           512
#define DEFAULT_DATA_SIZE 56

enum EOptions
{
	Verbose,
	Help,
};

typedef struct Option
{
	bool  isActive;
	bool  hasParam;
	char *param;
} option_t;

typedef struct Host
{
	int                id;
	char              *name;
	struct sockaddr_in addr;
	char               ipstr[INET_ADDRSTRLEN];
} host_t;

typedef struct State
{
	host_t *hosts;
	uint8_t hostsCount;
	int     socket;
} state_t;

#endif
