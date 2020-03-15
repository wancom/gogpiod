#include "gogpiod.h"
#include "_cgo_export.h"
#include <errno.h>

char *dev="";
char *appname="gpiod";
struct gpiod_chip *gchip=NULL;

int watchGPIO(unsigned int gpio,int cnt){
	struct gpiod_line *gl;
	struct gpiod_line_event ge;
	struct pollfd fds;
	int fd,ret;

	if (gchip == NULL){
		errno=EBADF;
		return -1;
	}

	if ((gl=gpiod_chip_get_line(gchip, gpio))==NULL){
		return -1;
	}

	if (gpiod_line_request_both_edges_events(gl, appname)==-1){
		return -1;
	}
	if ((fd=gpiod_line_event_get_fd(gl))==-1){
		return -1;
	}

	memset(&fds, 0, sizeof(fds));
	fds.fd = fd;
	fds.events = POLLIN | POLLPRI;

	while(!checkStop()){
		cnt = poll(&fds, 1, 1*1000);

		if (cnt < 0) return -1;
		else if (cnt == 0) continue;

		if (fds.revents) {
			if (gpiod_line_event_read(gl,&ge)==-1){
				return -1;
			}

			switch (ge.event_type) {
				case GPIOD_LINE_EVENT_RISING_EDGE:
					ret = intGPIO(gpio, 1 ,ge.ts.tv_sec,ge.ts.tv_nsec);
					break;
				case GPIOD_LINE_EVENT_FALLING_EDGE:
					ret = intGPIO(gpio, 0 ,ge.ts.tv_sec,ge.ts.tv_nsec);
					break;
				default:
				break;
			}
		}
	}

	return 0;
}

int setupGPIO(char *device, char *app){
	dev=device;
	appname=app;

  if ((gchip=gpiod_chip_open_lookup(dev)) == NULL) {
    return 1;
  }
	return 0;
}

void closeGPIO(){
	if (gchip != NULL)gpiod_chip_close(gchip);
}

int getGPIO(unsigned int offset){
	struct gpiod_line *gl;

	if (gchip == NULL){
		errno=EBADF;
		return -1;
	}

	if ((gl=gpiod_chip_get_line(gchip,offset))==NULL){
		return -1;
	}

	if (gpiod_line_request_input(gl,appname)==-1){
		return -1;
	}

	return gpiod_line_get_value(gl);
}

int setGPIO(unsigned int offset,int value){
	struct gpiod_line *gl;

	if (gchip == NULL){
		errno=EBADF;
		return -1;
	}

	if ((gl=gpiod_chip_get_line(gchip,offset))==NULL){
		return -1;
	}

	if (gpiod_line_request_output(gl,appname,0)==-1){
		return -1;
	}

	return gpiod_line_set_value(gl,value);
}