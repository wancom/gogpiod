package main

import (
	"fmt"
	"time"

	"github.com/wancom/gogpiod"
)

func main() {
	err = gogpiod.SetupGPIO("", "TestApp")
	if err != nil {
		panic(err)
	}

	v, err := gogpiod.GetGPIO(20)
	if err != nil {
		panic(err)
	}
	println(v)
	err = gogpiod.SetGPIO(21, 1)
	if err != nil {
		panic(err)
	}

	gpio := []int{5, 6}
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
		}
	}
}
