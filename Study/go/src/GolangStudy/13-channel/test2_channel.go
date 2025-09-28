package main

import (
	"fmt"
	"time"
)

func main() {
	c := make(chan int, 3)	// 带缓冲的channel

	fmt.Println("len(c) = ", len(c), " cap(c) = ", cap(c))

	go func() {
		defer fmt.Println("subgoroutine end")

		for i := 0; i < 3; i++ {
			c <- i
			fmt.Println("subgoroutine starting, 发送的元素 = ", i, " len(c) = ", len(c), " cap(c) = ", cap(c))
		}
	}()

	time.Sleep(2 * time.Second)

	for i := 0; i < 3; i++ {
		num := <-c
		fmt.Println("num = ", num)
	}

	fmt.Println("main end")
}