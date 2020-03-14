package gogpiod

// #cgo LDFLAGS: -lgpiod
/*
#include "gogpiod.h"
*/
import "C"
import (
	"time"
)

type GPIOEventInfo struct {
	Pin   int
	Value int
	Time  time.Time
}

var intch chan GPIOEventInfo
var stopch = make(chan int)

func SetupGPIO(device, appName string) {
	C.setupGPIO(C.CString(device), C.CString(appName))
}

func SetupWatchGPIO(gpio []int) chan GPIOEventInfo {
	if intch != nil {
		stopch <- 1
	}
	cgpio := make([]C.uint, len(gpio))
	for i, v := range gpio {
		cgpio[i] = C.uint(v)
	}
	intch = make(chan GPIOEventInfo, 100)
	go C.watchGPIO((*C.uint)(&cgpio[0]), C.int(len(gpio)))
	return intch
}

//export intGPIO
func intGPIO(pin int, value int, sec int64, nsec int64) int {
	intch <- GPIOEventInfo{pin, value, time.Unix(sec, nsec)}
	return checkStop()
}

//export checkStop
func checkStop() int {
	select {
	case v := <-stopch:
		if v != 0 {
			close(intch)
			return 1
		}
	default:
	}
	return 0
}

func GetGPIO(pin int) int {
	v := C.getGPIO(C.uint(pin))
	return int(v)
}

func SetGPIO(pin, value int) int {
	v := C.setGPIO(C.uint(pin), C.int(value))
	return int(v)
}
