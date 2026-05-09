#ifndef DEFS_H
#define DEFS_H

#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>

#define VALID_OPTIONS_NB  8
#define BUFSIZE           1500
#define DEFAULT_DATA_SIZE 56

enum EOptions
{
	Verbose,
	Help,
	Count,
	Quiet,
	Timeout,
	Interval,
	Size,
	Pattern
};

typedef struct Option
{
	bool  isActive;
	bool  hasParam;
	char *param;
} option_t;

typedef struct Host
{
	char              *name;
	struct sockaddr_in addr;
	char               ipstr[INET_ADDRSTRLEN];
} host_t;

typedef struct State
{
	host_t  *hosts;
	uint8_t  hostsCount;
	option_t options[VALID_OPTIONS_NB];
	int      socket;
} state_t;

#endif
