package main

import (
	"fmt"
	"time"

	"github.com/wancom/gogpiod"
)

func main() {
	gpio := []int{5, 6}
	gogpiod.SetupGPIOD("", "TestApp")
	println(gogpiod.GetGPIO(20))
	time.Sleep(5 * time.Second)
	ich := gogpiod.SetupWatchGPIO(gpio)
	go func() {
		time.Sleep(10 * time.Second)
		ich = gogpiod.SetupWatchGPIO(gpio)
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
