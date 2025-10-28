package main

import "fmt"

/*
切片的追加和截取
*/
func main() {
	s := make([]int, 3, 5)
	fmt.Printf("len = %d, cap = %d, s = %v\n", len(s), cap(s), s)

	// 追加
	// 超出cap，cap*2
	s = append(s, 1)
	fmt.Printf("len = %d, cap = %d, s = %v\n", len(s), cap(s), s)

	s[0] = 100

	// 截取 s1与s指向同一个数组
	s1 := s[0:2]
	fmt.Printf("len = %d, cap = %d, s1 = %v\n", len(s1), cap(s1), s1)

	// copy s2再开辟新数组
	var s2 []int
	copy(s2, s)
}