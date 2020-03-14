#ifndef test_h
#define test_h
#include <poll.h>
#include <gpiod.h>
int watchGPIO(unsigned int *gpio,int cnt);
int setupGPIO(char *device, char *app);
void closeGPIO();
int getGPIO(unsigned int offset);
int setGPIO(unsigned int offset,int value);
#endif