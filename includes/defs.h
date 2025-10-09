#ifndef DEFS_H
#define DEFS_H

#define VALID_OPTIONS_NB 2

enum EOptions
{
	Verbose,
	Help,
};

typedef struct State
{
	char **hosts;
} state_t;

#endif
