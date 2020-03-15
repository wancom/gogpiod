package gogpiod

// #cgo LDFLAGS: -lgpiod
/*
#include "gogpiod.h"
*/
import "C"
import (
	"time"
)

// GPIOEventInfo holds gpio event information(Pin, Value, and timestamp)
type GPIOEventInfo struct {
	Pin   int
	Value int
	Time  time.Time
}

var intch chan GPIOEventInfo
var stopch = make(chan int)

// SetupGPIO initialize GPIO and opens device
func SetupGPIO(device, appName string) error {
	r, err := C.setupGPIO(C.CString(device), C.CString(appName))
	if r != 0 {
		return err
	}
	return nil
}

// CloseGPIO closes device
func CloseGPIO() {
	C.closeGPIO()
}

// GetGPIO sets GPIO pin input mode and return GPIO value
func GetGPIO(pin int) (int, error) {
	v, err := C.getGPIO(C.uint(pin))
	if v == -1 {
		return -1, err
	}
	return int(v), nil
}

// SetGPIO sets GPIO pin output mode and set GPIO value
func SetGPIO(pin, value int) error {
	r, err := C.setGPIO(C.uint(pin), C.int(value))
	if r != 0 {
		return err
	}
	return nil
}

// WatchGPIO watches GPIO and return event channel
func WatchGPIO(gpio []int) chan GPIOEventInfo {
	if intch != nil {
		stopch <- 1
	}
	cgpio := make([]C.uint, len(gpio))
	for i, v := range gpio {
		cgpio[i] = C.uint(v)
	}
	intch = make(chan GPIOEventInfo, 100)
	go func() {
		C.watchGPIO((*C.uint)(&cgpio[0]), C.int(len(gpio)))
	}()
	return intch
}

//export intGPIO
func intGPIO(pin int, value int, sec int64, nsec int64) {
	intch <- GPIOEventInfo{pin, value, time.Unix(sec, nsec)}
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
