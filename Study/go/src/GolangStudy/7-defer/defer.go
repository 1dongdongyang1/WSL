package main

import "fmt"

/*
1. 多个defer，压栈，先进后出
2. defer和return谁先执行，return先执行
*/

func main() {
	// 写入defer关键字	-> 类析构
	defer fmt.Println("main end 1")

	fmt.Println("main::hello go 1")
	fmt.Println("main::hello go 2")
}
