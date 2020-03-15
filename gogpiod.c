#include "gogpiod.h"
#include <errno.h>
#include <gpiod.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>

#include "_cgo_export.h"

char *dev = "";
char *appname = "gpiod";
struct gpiod_chip *gchip = NULL;

int setupGPIO(char *device, char *app) {
  dev = device;
  appname = app;

  if ((gchip = gpiod_chip_open_lookup(dev)) == NULL) {
    return -1;
  }
  return 0;
}

void closeGPIO() {
  if (gchip != NULL) gpiod_chip_close(gchip);
}

int getGPIO(unsigned int offset) {
  struct gpiod_line *gl;

  if (gchip == NULL) {
    errno = EBADF;
    return -1;
  }

  if ((gl = gpiod_chip_get_line(gchip, offset)) == NULL) {
    return -1;
  }

  if (!gpiod_line_is_free(gl)) {
    if (!gpiod_line_is_requested(gl)) {
      errno = EBUSY;
      return -1;
    }
    gpiod_line_release(gl);
  }

  if (gpiod_line_request_input(gl, appname) == -1) {
    return -1;
  }

  return gpiod_line_get_value(gl);
}

int setGPIO(unsigned int offset, int value) {
  struct gpiod_line *gl;

  if (gchip == NULL) {
    errno = EBADF;
    return -1;
  }

  if ((gl = gpiod_chip_get_line(gchip, offset)) == NULL) {
    return -1;
  }

  if (!gpiod_line_is_free(gl)) {
    if (!gpiod_line_is_requested(gl)) {
      errno = EBUSY;
      return -1;
    }
    gpiod_line_release(gl);
  }

  if (gpiod_line_request_output(gl, appname, 0) == -1) {
    return -1;
  }

  return gpiod_line_set_value(gl, value);
}

int watchGPIO(unsigned int *gpio, int cnt) {
  struct gpiod_line *gl;
  struct gpiod_line_event ge;
  struct pollfd *fds;
  int i, ret;

  if (gchip == NULL) {
    errno = EBADF;
    return -1;
  }

  fds = malloc(sizeof(struct pollfd) * cnt);
  if (fds == NULL) return -1;
  memset(fds, 0, sizeof(struct pollfd) * cnt);

  for (i = 0; i < cnt, i++) {
    if ((gl = gpiod_chip_get_line(gchip, gpio[i])) == NULL) {
      free(fds);
      return -1;
    }

    if (!gpiod_line_is_free(gl)) {
      if (!gpiod_line_is_requested(gl)) {
        free(fds);
        errno = EBUSY;
        return -1;
      }
      gpiod_line_release(gl);
    }

    if (gpiod_line_request_both_edges_events(gl, appname) == -1) {
      free(fds);
      return -1;
    }

    if ((fds[i].fd = gpiod_line_event_get_fd(gl)) == -1) {
      free(fds);
      return -1;
    }
    fds[i].events = POLLIN | POLLPRI;
  }

  while (!checkStop()) {
    ret = poll(fds, cnt, 1 * 1000);

    if (ret < 0) {
      if (errno == EINTR) continue;  // Ignore signal
      free(fds);
      return -1;
    } else if (ret == 0)
      continue;

    for (i = 0; i < cnt, i++) {
      if (fds[i].revents) {
        if (gpiod_line_event_read_fd(fds[i].fd, &ge) == -1) {
          free(fds);
          return -1;
        }

        switch (ge.event_type) {
          case GPIOD_LINE_EVENT_RISING_EDGE:
            intGPIO(gpio, 1, ge.ts.tv_sec, ge.ts.tv_nsec);
            break;
          case GPIOD_LINE_EVENT_FALLING_EDGE:
            intGPIO(gpio, 0, ge.ts.tv_sec, ge.ts.tv_nsec);
            break;
          default:
            break;
        }
        ret--;
        if (ret == 0) break;
      }
    }
  }

  free(fds);
  return 0;
}