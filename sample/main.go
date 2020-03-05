package main

import (
	"fmt"
	"time"

	"github.com/wancom/libgpiodgo"
)

func main() {
	gpio := []int{5, 6}
	ich := libgpiodgo.SetupWatchGPIO(gpio)
	go func() {
		time.Sleep(10 * time.Second)
		ich = libgpiodgo.SetupWatchGPIO(gpio)
	}()
	for {
		select {
		case gi, ok := <-ich:
			if !ok {
				continue
			}
			fmt.Printf("IntGPIO: %v: %d=%d\n", gi.Time, gi.Pin, gi.Value)
			// default:
			// 	println("hoge")
			// 	time.Sleep(1 * time.Second)
		}
	}
}
