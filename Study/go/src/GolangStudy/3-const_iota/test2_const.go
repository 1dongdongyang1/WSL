package main

import "fmt"

// const 来定义枚举类型
const (
	// 可以在const()添加一个关键字iota，每行iota都会累加1，第一行iota默认值为0
	BEIJING = iota
	SHANGHAI = 2
	SHENZHEN
)

const (
	a, b = iota+1, iota+2
	c, d
	e, f 

)

func main() {
	// 常量
	const length int = 10
	fmt.Println("length = ", length)

	fmt.Println("a = ", BEIJING)
	fmt.Println("a = ", SHANGHAI)
	fmt.Println("a = ", SHENZHEN)

	// iota只能在const()里使用
	// var a = iota
}
