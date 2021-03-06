package gogpiod

import (
	"fmt"
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
	fmt.Printf("SetupGPIO: device=%s, appName=%s\n", device, appName)
	return nil
}

// CloseGPIO closes device
func CloseGPIO() {
	fmt.Printf("CloseGPIO\n")
}

// GetGPIO sets GPIO pin input mode and return GPIO value
func GetGPIO(pin int) (int, error) {
	fmt.Printf("GetGPIO: pin=%d\n", pin)
	return 0, nil
}

// SetGPIO sets GPIO pin output mode and set GPIO value
func SetGPIO(pin, value int) error {
	fmt.Printf("SetGPIO: pin=%d, value=%d\n", pin, value)
	return nil
}

// WatchGPIO watches GPIO and return event channel
func WatchGPIO(gpio []int) chan GPIOEventInfo {
	fmt.Printf("WatchGPIO: gpio=%v\n", gpio)
	intch = make(chan GPIOEventInfo, 100)
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
