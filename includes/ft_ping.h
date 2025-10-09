#ifndef FT_PING_H
#define FT_PING_H

#include <stdbool.h>
#define VALID_OPTIONS_NB 2

enum EOptions
{
	Verbose,
	Help,
};

// OUTPUT

void invalidOption(char option);
void unrecognizedOption(const char *option);
void help(void);
void error(const char *type, const char *msg);
void fatalError(const char *type, const char *msg);

// ARGS

void   getOptions(char **args, bool *options);
char **getHosts(char **args);

#endif
