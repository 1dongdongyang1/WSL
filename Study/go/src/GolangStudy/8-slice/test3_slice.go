package main

import "fmt"

func main() {
	// 声明slice1是一个切片，并且初始化，默认值为1，2，3，长度为3
	// slice1 := []int{1,2,3}

	// 声明slice1是一个切片，但是没有给slice1分配空间
	// var slice1 []int
	// slice1 = make([]int, 3) // 开辟3个空间，默认值为0

	// var slice1 []int = make([]int, 3)

	slice1 := make([]int, 3)

	fmt.Printf("len = %d, slice = %v\n", len(slice1), slice1)

	// 判断一个slice是否有空间
	if slice1 == nil {
		fmt.Println("slice 是空切片")
	} else {
		fmt.Println("slice 有空间")
	}
}
