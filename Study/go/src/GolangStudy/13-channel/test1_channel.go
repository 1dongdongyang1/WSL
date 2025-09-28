package main

import "fmt"

func main() {
	// 定义一个channel
	c := make(chan int)

	go func() {
		defer fmt.Println("goroutine end")

		fmt.Println("goroutine starting...")

		c <- 666
	}()

	num := <-c

	fmt.Println("num = ", num)
	fmt.Println("main goroutine end")
}
