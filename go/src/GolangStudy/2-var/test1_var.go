package main

import "fmt"
/*
	四种变量声明方式
*/

// 声明全局变量 1,2,3可以
var gA int = 100
var gB = 200
// 4不行，只能在函数体内使用
// gC := 300

func main() {
	// 1.声明一个变量 默认值为0
	var a int 
	fmt.Println("a = ", a)	

	// 2.声明一个变量，初始化一个值
	var b int = 100
	fmt.Println("b = ", b)

	// 3.在初始化时，省去数据类型，通过值自动匹配当前变量的数据类型
	var c = 100
	fmt.Println("c = ", c)

	// 4.(最常用)省去var，直接自动匹配
	d := 100
	fmt.Printf("d = %d\n", d)

	fmt.Printf("gA = %d gB = %d\n", gA, gB)

	// 声明多个变量
	var xx, yy int = 100, 200
	fmt.Println("xx = ", xx, "yy = ", yy)
	var ii, jj = 100, "ddy"
	fmt.Println("ii = ", ii, "jj = ", jj)

	var (
		vv int = 100
		hh bool = true
	)
	fmt.Println("vv = ", vv, "hh = ", hh)
}