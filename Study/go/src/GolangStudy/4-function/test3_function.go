package main

import "fmt"

func foo1(a string, b int) int {
	fmt.Println("a = ", a)
	fmt.Println("b = ", b)

	c := 100

	return c
}

// 返回多个返回值，匿名
func foo2(a string, b int) (int, int) {
	fmt.Println("a = ", a)
	fmt.Println("b = ", b)

	return 66, 77
}

// 有形参名称
func foo3(a string, b int) (r1 int, r2 int) {
	fmt.Println("a = ", a)
	fmt.Println("b = ", b)

	r1 = 100
	r2 = 200

	return 
}

func foo4(a string, b int) (r1, r2 int) {
	fmt.Println("a = ", a)
	fmt.Println("b = ", b)

	r1 = 100
	r2 = 200

	return 
}

func main() {
	c := foo1("ddy", 200)
	fmt.Println("c =", c)

	ret1, ret2 := foo2("ym", 300)
	fmt.Println(ret1, ret2)

	ret1, ret2 = foo3("foo3", 1)
	ret1, ret2 = foo4("foo3", 1)
}
