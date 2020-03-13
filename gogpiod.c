#include "gogpiod.h"
#include "_cgo_export.h"

char *dev="";
char *appname="gpiod";

static int poll_callback(unsigned int num_lines, struct gpiod_ctxless_event_poll_fd *fds, const struct timespec *timeout, void *data) {
	struct pollfd pfds[GPIOD_LINE_BULK_MAX_LINES + 1];
	int cnt;
	unsigned int i;

	for (i = 0; i < num_lines; i++) {
		pfds[i].fd = fds[i].fd;
		pfds[i].events = POLLIN | POLLPRI;
	}

	cnt = poll(pfds, num_lines, timeout->tv_sec * 1000 + timeout->tv_nsec / 1000000);

	if (cnt < 0) return GPIOD_CTXLESS_EVENT_POLL_RET_ERR;
	else if (cnt == 0) {
    if (checkStop()) return GPIOD_CTXLESS_EVENT_POLL_RET_STOP;
    return GPIOD_CTXLESS_EVENT_POLL_RET_TIMEOUT;
  }

	for (i = 0; i < num_lines; i++) {
		if (pfds[i].revents) fds[i].event = true;
	}

  return cnt;
}

static int event_callback(int event_type, unsigned int line_offset, const struct timespec *timestamp, void *data) {
  int ret = 0;

  switch (event_type) {
    case GPIOD_CTXLESS_EVENT_CB_RISING_EDGE:
      ret = intGPIO(line_offset, 1 ,timestamp->tv_sec,timestamp->tv_nsec);
      break;
    case GPIOD_CTXLESS_EVENT_CB_FALLING_EDGE:
      ret = intGPIO(line_offset, 0 ,timestamp->tv_sec,timestamp->tv_nsec);
      break;
    default:
      return GPIOD_CTXLESS_EVENT_CB_RET_OK;
	}
	if (ret) return GPIOD_CTXLESS_EVENT_CB_RET_STOP;
	return GPIOD_CTXLESS_EVENT_CB_RET_OK;
}
int watchGPIO(unsigned int *gpio,int cnt){
	struct timespec timeout = { 1, 0 };
  return gpiod_ctxless_event_monitor_multiple(dev, GPIOD_CTXLESS_EVENT_BOTH_EDGES, gpio, cnt, false, appname, &timeout, poll_callback, event_callback, NULL);
}

int setupGPIOD(char *device, char *app){
	dev=device;
	appname=app;
}
