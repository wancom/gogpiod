package main

import (
	"fmt"

	"github.com/wancom/gogpiod"
)

func main() {
	gogpiod.SetupGPIO("", "TestApp")

	println(gogpiod.GetGPIO(20))
	gogpiod.SetGPIO(21, 1)

	gpio := []int{5, 6}
	ich := gogpiod.SetupWatchGPIO(gpio)
	// go func() {
	// 	time.Sleep(10 * time.Second)
	// 	ich = gogpiod.SetupWatchGPIO(gpio)
	// }()
	for {
		select {
		case gi, ok := <-ich:
			if !ok {
				continue
			}
			fmt.Printf("IntGPIO: %v: %d=%d\n", gi.Time, gi.Pin, gi.Value)
		}
	}
}
