#ifndef FT_PING_H
#define FT_PING_H

#include "defs.h"
#include <stdbool.h>

void help(void);

void error(const char *msg, state_t *state, bool closeSocket);
void argError(const char *msg);
void fatalError(const char *msg, state_t *state, bool closeSocket);
void invalidOption(char option);
void unrecognizedOption(const char *option);

void statistics(const char *hostname, uint32_t packetsSent, uint32_t packetsReceived, float *timeStats);
void receivedPacket(uint16_t packetSize, const char *ipstr, uint32_t seq, uint32_t ttl, float time);

void    getOptions(int argc, char **argv, option_t *options);
host_t *getHosts(char **args, uint8_t hostsCount);
uint8_t countHosts(char **args);

void setSignalHandlers(state_t *state);
bool checkSIGINT(state_t *state);

void freeState(state_t *state);

void createEchoRequest(char *packet);

void ping(state_t *state);

#endif
