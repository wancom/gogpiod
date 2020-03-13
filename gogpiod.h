#ifndef test_h
#define test_h
#include <poll.h>
#include <gpiod.h>
int watchGPIO(unsigned int *gpio,int cnt);
int setupGPIOD(char *device, char *app);
int getGPIO(unsigned int offset);
int setGPIO(unsigned int offset,int value);
#endif