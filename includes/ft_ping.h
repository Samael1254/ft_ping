#ifndef FT_PING_H
#define FT_PING_H

#include "defs.h"
#include <stdbool.h>

void invalidOption(char option);
void unrecognizedOption(const char *option);
void help(void);
void error(const char *type, const char *msg);
void fatalError(const char *type, const char *msg);

void   getOptions(char **args, bool *options);
char **getHosts(char **args);

void setSignalHandlers();
bool checkSIGINT();

void freeState(state_t *state);

void ping(state_t *state);

#endif
